#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "SimulationConfig.hh" // <-- Added

#include <filesystem>
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh" // Needed for G4cerr and G4endl

#include <fstream>
#include <iomanip>
#include <sstream>

RunAction* RunAction::fgInstance = nullptr;

// Updated constructor to take the SimulationConfig pointer
RunAction::RunAction(const DetectorConstruction* det,
                     const SimulationConfig* config)
  : G4UserRunAction(), fDet(det), fConfig(config)
{
  fgInstance = this;
  fBinWidth = (fEmax - fEmin) / fNBins;
  fSpectra.assign(fDet->GetNumberOfDetectors(),
                  std::vector<G4double>(fNBins, 0.0));
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
  // Intentionally no reset here:
  // chunked BeamOn() calls should accumulate.
}

void RunAction::EndOfRunAction(const G4Run*)
{
  // No-op; checkpoint writing is triggered from main().
}

void RunAction::ResetAll()
{
  for (auto& detSpec : fSpectra) {
    std::fill(detSpec.begin(), detSpec.end(), 0.0);
  }
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

void RunAction::WriteCheckpoint(long long cumulativeEvents) const
{
  const std::string filename = fConfig->BuildCheckpointFilename(cumulativeEvents);

  std::filesystem::path outPath(filename);
  
  // Safely create directories only if there is a parent path (e.g., "output/")
  if (outPath.has_parent_path()) {
    std::filesystem::create_directories(outPath.parent_path());
  }

  std::ofstream out(filename);
  if (!out) {
    G4cerr << "ERROR: could not open output file: " << filename << G4endl;
    return;
  }
  
  out << std::fixed << std::setprecision(6);

  // Updated header to include beam energy and fixed distance units
  out << "# run_label,"          << fConfig->BuildRunLabel() << "\n";
  out << "# processed_events,"   << cumulativeEvents << "\n";
  out << "# sample_material,"    << fConfig->sampleMaterial << "\n";
  out << "# beam_energy_keV,"    << fConfig->beamEnergy / keV << "\n";
  out << "# incident_angle_deg," << fDet->GetIncidentAngleDeg() << "\n";
  out << "# source_distance_mm," << fDet->GetSourceDistance() / mm << "\n";
  out << "# detector_distance_mm,"<< fDet->GetDetectorDistance() / mm << "\n";
  out << "# nominal_takeoff_deg,"<< fDet->GetNominalTakeoffDeg() << "\n";
  out << "# detector_spread_deg,"<< fDet->GetDetectorSpreadDeg() << "\n";
  out << "# detector_step_deg,"  << fDet->GetDetectorStepDeg() << "\n";
  out << "# columns: det_id,det_angle_deg,bin_idx,bin_low_keV,bin_high_keV,count\n";

  const auto& angles = fDet->GetDetectorAnglesDeg();

  for (G4int detId = 0; detId < static_cast<G4int>(fSpectra.size()); ++detId) {
    for (G4int b = 0; b < fNBins; ++b) {
      const G4double eLow  = (fEmin + b * fBinWidth) / keV;
      const G4double eHigh = (fEmin + (b + 1) * fBinWidth) / keV;

      out << detId << ","
          << angles[detId] << ","
          << b << ","
          << eLow << ","
          << eHigh << ","
          << fSpectra[detId][b] << "\n";
    }
  }
}