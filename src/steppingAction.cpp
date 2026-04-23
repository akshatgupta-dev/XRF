#include "steppingAction.hh"

#include "G4Step.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"

SteppingAction::SteppingAction() {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto* track = step->GetTrack();
    if (track->GetDefinition() == G4Gamma::GammaDefinition() &&
        track->GetCurrentStepNumber() == 1 &&
        track->GetParentID() > 0 &&
        track->GetCreatorProcess() &&
        track->GetCreatorProcess()->GetProcessName() == "phot") {

        auto* birthLV = track->GetLogicalVolumeAtVertex();
        G4String birthName = birthLV ? birthLV->GetName() : "Unknown";

        // G4cout << "NEW GAMMA BORN: "
        //        << "E=" << track->GetKineticEnergy()/keV << " keV "
        //        << "birthVol=" << birthName << " "
        //        << "birthPos=" << track->GetVertexPosition()
        //        << G4endl;
    }
        
    auto* preVol = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
    if (!preVol) return;

    auto* preLV = preVol->GetLogicalVolume();
    if (!preLV) return;

    auto* proc = step->GetPostStepPoint()->GetProcessDefinedStep();
    if (!proc) return;

    // if (preLV->GetName() == "sampleLogic" && proc->GetProcessName() == "phot") {
    //     G4cout << "PHOTOELECTRIC in sample at "
    //            << step->GetPostStepPoint()->GetPosition()
    //            << G4endl;
    // }
}