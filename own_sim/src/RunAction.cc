#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "SimulationConfig.hh"
#include <algorithm>
#include <cmath>
#include <limits>
#include <filesystem>
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh" 
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include <fstream>
#include <iomanip>
#include <sstream>

RunAction* RunAction::fgInstance = nullptr;

namespace
{
  G4double ThetaDegFromVector(const G4ThreeVector& p)
  {
    const G4double rho = std::sqrt(p.x()*p.x() + p.y()*p.y());
    return std::atan2(p.z(), rho) / deg;
  }

  G4double PhiDegFromVector(const G4ThreeVector& p)
  {
    return std::atan2(p.y(), p.x()) / deg;
  }

  std::string JoinIds(const std::vector<G4int>& ids)
  {
    std::ostringstream oss;
    for (std::size_t i = 0; i < ids.size(); ++i) {
      if (i > 0) oss << ";";
      oss << ids[i];
    }
    return oss.str();
  }
}

// Updated constructor to take the SimulationConfig pointer
RunAction::RunAction(const DetectorConstruction* det,
                     const SimulationConfig* config)
  : G4UserRunAction(), fDet(det), fConfig(config)
{
  fgInstance = this;
  fBinWidth = (fEmax - fEmin) / fNBins;
  SyncWithDetectorLayout();
}

RunAction::~RunAction()
{
  if (fgInstance == this) {
    fgInstance = nullptr;
  }
}

RunAction* RunAction::Instance()
{
  return fgInstance;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  ResetShieldLayerDebug();
  // Intentionally no reset here:
  // chunked BeamOn() calls should accumulate.
}

void RunAction::SyncWithDetectorLayout()
{
  fSpectra.assign(
      fDet->GetNumberOfDetectors(),
      std::vector<G4double>(fNBins, 0.0)
  );

  RebuildCombos();
}

void RunAction::EndOfRunAction(const G4Run*)
{

  G4cout << G4endl;
G4cout << "==== Shield layer debug summary ====" << G4endl;

if (fShieldLayerEntries.empty()) {
    G4cout << "No shield layer entries recorded." << G4endl;
} else {
    for (const auto& kv : fShieldLayerEntries) {
        const G4int layer = kv.first;
        const G4long entries = kv.second;
        const G4double edep = fShieldLayerEdep[layer];

        G4cout << "Layer " << layer
               << " : entries = " << entries
               << ", edep = " << edep / keV << " keV"
               << G4endl;
    }
}

G4cout << "====================================" << G4endl;
  // No-op; checkpoint writing is triggered from main().
}

void RunAction::RebuildCombos()
{
  fCombos.clear();

  const auto& elements = fDet->GetDetectorElements();
  if (elements.empty()) {
    return;
  }

  struct RankedDet {
    G4double score;
    G4int id;
  };

  std::vector<RankedDet> ranked;
  ranked.reserve(elements.size());

  const G4double theta0 = fDet->GetNominalTakeoffDeg();
  const G4double phi0 = 0.0;

  for (const auto& elem : elements) {
    const G4double dTheta = elem.thetaDeg - theta0;
    const G4double dPhi   = elem.phiDeg - phi0;
    const G4double score  = std::sqrt(dTheta*dTheta + dPhi*dPhi);

    ranked.push_back({score, elem.id});
  }

  std::sort(ranked.begin(), ranked.end(),
            [](const RankedDet& a, const RankedDet& b) {
              if (a.score != b.score) return a.score < b.score;
              return a.id < b.id;
            });

  std::vector<G4int> comboSizes = fConfig->comboSizes;
  if (comboSizes.empty()) {
    comboSizes = {1, 3, 5, 9};
  }
  comboSizes.push_back(static_cast<G4int>(ranked.size()));

  std::sort(comboSizes.begin(), comboSizes.end());
  comboSizes.erase(std::unique(comboSizes.begin(), comboSizes.end()), comboSizes.end());

  for (G4int n : comboSizes) {
    if (n <= 0 || n > static_cast<G4int>(ranked.size())) {
      continue;
    }

    DetectorCombo combo;
    
    // Updated combo naming logic
    if (n == static_cast<G4int>(ranked.size())) {
      combo.name = "nearest_all";
    } else {
      combo.name = "nearest_" + std::to_string(n);
    }

    for (G4int i = 0; i < n; ++i) {
      combo.detIds.push_back(ranked[i].id);
    }

    std::sort(combo.detIds.begin(), combo.detIds.end());
    fCombos.push_back(combo);
  }

  // Configured custom combos
  for (const auto& cfgCombo : fConfig->customCombos) {
    if (cfgCombo.name.empty() || cfgCombo.detIds.empty()) {
      continue;
    }

    DetectorCombo combo;
    combo.name = cfgCombo.name;

    for (G4int detId : cfgCombo.detIds) {
      if (detId >= 0 && detId < static_cast<G4int>(elements.size())) {
        combo.detIds.push_back(detId);
      }
    }

    std::sort(combo.detIds.begin(), combo.detIds.end());
    combo.detIds.erase(std::unique(combo.detIds.begin(), combo.detIds.end()),
                       combo.detIds.end());

    if (!combo.detIds.empty()) {
      fCombos.push_back(combo);
    }
  }

  G4cout << "DEBUG RebuildCombos: built " << fCombos.size()
         << " detector combos." << G4endl;
}

void RunAction::ResetAll()
{
  for (auto& detSpec : fSpectra) {
    std::fill(detSpec.begin(), detSpec.end(), 0.0);
  }
}

std::vector<G4double> RunAction::SumSpectrum(const std::vector<G4int>& detIds) const
{
  std::vector<G4double> summed(fNBins, 0.0);

  for (G4int detId : detIds) {
    if (detId < 0 || detId >= static_cast<G4int>(fSpectra.size())) {
      continue;
    }

    for (G4int b = 0; b < fNBins; ++b) {
      summed[b] += fSpectra[detId][b];
    }
  }

  return summed;
}

void RunAction::ResetShieldLayerDebug()
{
    fShieldLayerEntries.clear();
    fShieldLayerEdep.clear();
}

void RunAction::RecordShieldLayerEntry(G4int layer, G4double edep)
{
    fShieldLayerEntries[layer]++;
    fShieldLayerEdep[layer] += edep;
}

void RunAction::ScorePhoton(G4int detId, G4double energy)
{
  if (detId < 0 || detId >= static_cast<G4int>(fSpectra.size())) {
    return;
  }

  if (energy < fEmin || energy >= fEmax) {
    return;
  }

  const G4int bin = static_cast<G4int>((energy - fEmin) / fBinWidth);
  if (bin >= 0 && bin < fNBins) {
    fSpectra[detId][bin] += 1.0;
  }
}

std::string RunAction::BuildComboFilename(long long cumulativeEvents) const
{
  std::filesystem::path p(fConfig->BuildCheckpointFilename(cumulativeEvents));
  const std::string stem = p.stem().string();
  const std::string ext  = p.extension().string();

  return (p.parent_path() / (stem + "_combos" + ext)).string();
}

void RunAction::WriteComboCheckpoint(long long cumulativeEvents) const
{
  const auto& elements = fDet->GetDetectorElements();
  const std::string filename = BuildComboFilename(cumulativeEvents);

  std::filesystem::path outPath(filename);
  if (outPath.has_parent_path()) {
    std::filesystem::create_directories(outPath.parent_path());
  }

  std::ofstream out(filename);
  if (!out) {
    G4cerr << "ERROR: could not open combo output file: " << filename << G4endl;
    return;
  }

  out << std::fixed << std::setprecision(6);

  out << "# run_label,"            << fConfig->BuildRunLabel() << "\n";
  out << "# processed_events,"     << cumulativeEvents << "\n";
  out << "# sample_material,"      << fConfig->sampleMaterial << "\n";
  out << "# beam_energy_keV,"      << fConfig->beamEnergy / keV << "\n";
  out << "# incident_angle_deg,"   << fDet->GetIncidentAngleDeg() << "\n";
  out << "# source_distance_mm,"   << fDet->GetSourceDistance() / mm << "\n";
  out << "# detector_distance_mm," << fDet->GetDetectorDistance() / mm << "\n";
  out << "# nominal_takeoff_deg,"  << fDet->GetNominalTakeoffDeg() << "\n";
  out << "# detector_spread_deg,"  << fDet->GetDetectorSpreadDeg() << "\n";
  out << "# detector_step_deg,"    << fDet->GetDetectorStepDeg() << "\n";
  out << "# columns: combo_name,combo_size,detector_ids,theta_eff_deg,phi_eff_deg,theta_min_deg,theta_max_deg,phi_min_deg,phi_max_deg,bin_idx,bin_low_keV,bin_high_keV,count\n";

  for (const auto& combo : fCombos) {
    if (combo.detIds.empty()) {
      continue;
    }

    auto comboSpec = SumSpectrum(combo.detIds);

    G4double thetaMin = std::numeric_limits<G4double>::max();
    G4double thetaMax = -std::numeric_limits<G4double>::max();
    G4double phiMin   = std::numeric_limits<G4double>::max();
    G4double phiMax   = -std::numeric_limits<G4double>::max();

    G4ThreeVector sumDir(0, 0, 0);

    for (G4int detId : combo.detIds) {
      if (detId < 0 || detId >= static_cast<G4int>(elements.size())) {
        continue;
      }

      const auto& elem = elements.at(detId);

      thetaMin = std::min(thetaMin, elem.thetaDeg);
      thetaMax = std::max(thetaMax, elem.thetaDeg);
      phiMin   = std::min(phiMin, elem.phiDeg);
      phiMax   = std::max(phiMax, elem.phiDeg);

      sumDir += elem.center.unit();
    }

    G4double thetaEff = 0.0;
    G4double phiEff   = 0.0;
    if (sumDir.mag() > 0.0) {
      thetaEff = ThetaDegFromVector(sumDir.unit());
      phiEff   = PhiDegFromVector(sumDir.unit());
    }

    const std::string detIdList = JoinIds(combo.detIds);

    for (G4int b = 0; b < fNBins; ++b) {
      const G4double eLow  = (fEmin + b * fBinWidth) / keV;
      const G4double eHigh = (fEmin + (b + 1) * fBinWidth) / keV;

      out << combo.name << ","
          << combo.detIds.size() << ","
          << "\"" << detIdList << "\"" << ","
          << thetaEff << ","
          << phiEff << ","
          << thetaMin << ","
          << thetaMax << ","
          << phiMin << ","
          << phiMax << ","
          << b << ","
          << eLow << ","
          << eHigh << ","
          << comboSpec[b] << "\n";
    }
  }
}

void RunAction::WriteCheckpoint(long long cumulativeEvents) const
{
  const auto& elements = fDet->GetDetectorElements();
  const G4int nDet = std::min<G4int>(fSpectra.size(), elements.size());

  G4cout << "DEBUG WriteCheckpoint: fSpectra.size() = " << fSpectra.size()
         << ", elements.size() = " << elements.size() << G4endl;

  if (fSpectra.size() != elements.size()) {
    G4cerr << "ERROR: detector layout mismatch. "
           << "fSpectra.size()=" << fSpectra.size()
           << " elements.size()=" << elements.size()
           << G4endl;
  }

  const std::string filename = fConfig->BuildCheckpointFilename(cumulativeEvents);

  std::filesystem::path outPath(filename);
  if (outPath.has_parent_path()) {
    std::filesystem::create_directories(outPath.parent_path());
  }

  std::ofstream out(filename);
  if (!out) {
    G4cerr << "ERROR: could not open output file: " << filename << G4endl;
    return;
  }

  out << std::fixed << std::setprecision(6);

  out << "# run_label,"          << fConfig->BuildRunLabel() << "\n";
  out << "# processed_events,"   << cumulativeEvents << "\n";
  out << "# sample_material,"    << fConfig->sampleMaterial << "\n";
  out << "# beam_energy_keV,"    << fConfig->beamEnergy / keV << "\n";
  out << "# incident_angle_deg," << fDet->GetIncidentAngleDeg() << "\n";
  out << "# source_distance_mm," << fDet->GetSourceDistance() / mm << "\n";
  out << "# detector_distance_mm,"<< fDet->GetDetectorDistance() / mm << "\n";
  out << "# nominal_takeoff_deg," << fDet->GetNominalTakeoffDeg() << "\n";
  out << "# detector_spread_deg," << fDet->GetDetectorSpreadDeg() << "\n";
  out << "# detector_step_deg,"   << fDet->GetDetectorStepDeg() << "\n";
  out << "# columns: det_id,theta_deg,phi_deg,x_mm,y_mm,z_mm,bin_idx,bin_low_keV,bin_high_keV,count\n";

  for (G4int detId = 0; detId < nDet; ++detId) {
    const auto& elem = elements.at(detId);

    for (G4int b = 0; b < fNBins; ++b) {
      const G4double eLow  = (fEmin + b * fBinWidth) / keV;
      const G4double eHigh = (fEmin + (b + 1) * fBinWidth) / keV;

      out << detId << ","
          << elem.thetaDeg << ","
          << elem.phiDeg << ","
          << elem.center.x() / mm << ","
          << elem.center.y() / mm << ","
          << elem.center.z() / mm << ","
          << b << ","
          << eLow << ","
          << eHigh << ","
          << fSpectra[detId][b] << "\n";
    }
  }
  WriteComboCheckpoint(cumulativeEvents);
}