#include "DetectorSD.hh"
#include "RunAction.hh"

#include "G4Gamma.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"

DetectorSD::DetectorSD(const G4String& name)
  : G4VSensitiveDetector(name)
{
}

G4bool DetectorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  auto* track = step->GetTrack();
  if (track->GetDefinition() != G4Gamma::Gamma()) {
    return false;
  }

  auto* pre = step->GetPreStepPoint();

  // Count gamma only when it enters detector volume
  if (pre->GetStepStatus() != fGeomBoundary) {
    return false;
  }

  const G4int detId     = pre->GetTouchableHandle()->GetCopyNumber();
  const G4double energy = pre->GetKineticEnergy();

  auto* runAction = RunAction::Instance();
  if (runAction) {
    runAction->ScorePhoton(detId, energy);
  }

  // Virtual detector behavior: once counted, stop this track in the detector
  track->SetTrackStatus(fStopAndKill);
  return true;
}