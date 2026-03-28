#include "RunAction.hh"
#include "DetectorConstruction.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

#include <fstream>
#include <iomanip>
#include <sstream>

RunAction* RunAction::fgInstance = nullptr;

RunAction::RunAction(const DetectorConstruction* det)
  : fDet(det)
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
  std::ostringstream name;
  name << "checkpoint_" << cumulativeEvents << ".csv";

  std::ofstream out(name.str());
  out << std::fixed << std::setprecision(6);

  out << "# processed_events,"      << cumulativeEvents << "\n";
  out << "# sample_material,"       << fDet->GetSampleMaterial() << "\n";
  out << "# incident_angle_deg,"    << fDet->GetIncidentAngleDeg() << "\n";
  out << "# source_distance_mm,"    << fDet->GetSourceDistance() << "\n";
  out << "# detector_distance_mm,"  << fDet->GetDetectorDistance() << "\n";
  out << "# nominal_takeoff_deg,"   << fDet->GetNominalTakeoffDeg() << "\n";
  out << "# detector_spread_deg,"   << fDet->GetDetectorSpreadDeg() << "\n";
  out << "# detector_step_deg,"     << fDet->GetDetectorStepDeg() << "\n";
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