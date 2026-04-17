#include "PMSensitiveDetector.hh"
#include "RunAction.hh"

#include "G4Gamma.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4TouchableHandle.hh"
#include "G4ios.hh"

#include <cmath>

PMSensitiveDetector::PMSensitiveDetector(G4String name)
    : G4VSensitiveDetector(name),
    fTotalDetectedEnergy(0.)
{
}

PMSensitiveDetector::~PMSensitiveDetector()
{
}

void PMSensitiveDetector::Initialize(G4HCofThisEvent*)
{
    fTotalDetectedEnergy = 0.;
    fFluorescenceCountsByBin.clear();
    fCountedTrackIds.clear();
}

void PMSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    //G4cout << "Detected fluorescence energy: " << fTotalDetectedEnergy / keV << " keV" << G4endl;

    //if (fFluorescenceCountsByBin.empty()) {
     //   G4cout << "Fluorescence spectrum: no gamma entries in detector." << G4endl;
     //   return;
    //}

    //G4cout << "Fluorescence spectrum (bin center in keV, photons):" << G4endl;
    //for (const auto& [binIdx, count] : fFluorescenceCountsByBin) {
    //    const G4double centerKeV = (binIdx + 0.5) * kBinWidthKeV;
    //    G4cout << "  " << centerKeV << " keV : " << count << G4endl;
    //}
}

G4bool PMSensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    auto* track = aStep->GetTrack();
    if (track->GetDefinition() != G4Gamma::Gamma()) {
        return false;
    }

    const G4int trackId = track->GetTrackID();
    if (!fCountedTrackIds.insert(trackId).second) {
        return false;
    }

    auto* pre = aStep->GetPreStepPoint();
    const G4int detId = pre->GetTouchableHandle()->GetCopyNumber();

    const G4double energy = pre->GetKineticEnergy();
    fTotalDetectedEnergy += energy;

    auto* runAction = RunAction::Instance();
    if (runAction) {
        runAction->ScorePhoton(detId, energy);
    }

    const G4double energyKeV = energy / keV;
    const G4int binIdx = static_cast<G4int>(std::floor(energyKeV / kBinWidthKeV));
    fFluorescenceCountsByBin[binIdx] += 1;

    track->SetTrackStatus(fStopAndKill);

    return true;
}