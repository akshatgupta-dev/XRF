#include "DetectorSD.hh"
#include "RunAction.hh"

#include "G4AnalysisManager.hh"
#include "G4Gamma.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4TouchableHandle.hh"
#include "G4VPhysicalVolume.hh"
#include "G4HCofThisEvent.hh"

DetectorSD::DetectorSD(const G4String& name)
  : G4VSensitiveDetector(name)
{
}

void DetectorSD::Initialize(G4HCofThisEvent*)
{
  fTotalEnergyDeposited = 0.0;
}

G4bool DetectorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  auto* track = step->GetTrack();
  auto* pre   = step->GetPreStepPoint();

  // -------------------------------------------------
  // 1. Keep the old CSV-style scoring:
  //    count gamma energy when gamma ENTERS detector
  // -------------------------------------------------
  if (track->GetDefinition() == G4Gamma::Gamma() &&
      pre->GetStepStatus() == fGeomBoundary)
  {
    const G4int detId     = pre->GetTouchableHandle()->GetCopyNumber();
    const G4double energy = pre->GetKineticEnergy();

    auto* runAction = RunAction::Instance();
    if (runAction) {
      runAction->ScorePhoton(detId, energy);   // keeps CSV/root ntuple binning logic
    }

    // ROOT histogram: GammaInAll
    auto* analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillH1(1, energy / keV);
  }

  // -------------------------------------------------
  // 2. New: real deposited-energy scoring
  //    accumulate energy deposited in detector
  // -------------------------------------------------
  const G4double edep = step->GetTotalEnergyDeposit();
  if (edep > 0.0) {
    fTotalEnergyDeposited += edep;
  }

  // IMPORTANT:
  // Do NOT kill the track anymore, otherwise you cannot get Edep.
  // track->SetTrackStatus(fStopAndKill);

  return true;
}

void DetectorSD::EndOfEvent(G4HCofThisEvent*)
{
  if (fTotalEnergyDeposited <= 0.0) {
    return;
  }

  auto* analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillH1(0, fTotalEnergyDeposited / keV);
}