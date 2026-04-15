#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class G4Step;
class RunAction;

class SteppingAction : public G4UserSteppingAction
{
public:
    explicit SteppingAction(RunAction* runAction);
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step* step) override;

private:
    RunAction* fRunAction;

    G4int ExtractLayerIndex(const G4String& name) const;
};

#endif