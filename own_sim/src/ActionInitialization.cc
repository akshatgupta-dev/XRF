#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SimulationConfig.hh"       // <-- Added to include the config structure

// Updated constructor to take the config pointer
ActionInitialization::ActionInitialization(const DetectorConstruction* det,
                                           SimulationConfig* config)
  : G4VUserActionInitialization(), 
    fDet(det), 
    fConfig(config)
{
}

void ActionInitialization::BuildForMaster() const
{
  // Pass the config pointer to RunAction
  SetUserAction(new RunAction(fDet, fConfig));
}

void ActionInitialization::Build() const
{
  // Pass the config pointer to both RunAction and PrimaryGeneratorAction
  SetUserAction(new RunAction(fDet, fConfig));
  
  // Change to "new PMPrimaryGenerator(fDet, fConfig)" if that is your class name
  SetUserAction(new PrimaryGeneratorAction(fDet, fConfig)); 
}