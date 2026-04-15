#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "SimulationConfig.hh"       // <-- Added to include the config structure

// Updated constructor to take the config pointer
ActionInitialization::ActionInitialization(const DetectorConstruction* det,
                                           SimulationConfig* config)
  : G4VUserActionInitialization(), 
    fDet(det), 
    fConfig(config)
{
}

void ActionInitialization::Build() const
{
  auto* runAction = new RunAction(fDet, fConfig);
  SetUserAction(runAction);
  SetUserAction(new PrimaryGeneratorAction(fDet, fConfig));
  SetUserAction(new SteppingAction(runAction));
}

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction(fDet, fConfig));
}