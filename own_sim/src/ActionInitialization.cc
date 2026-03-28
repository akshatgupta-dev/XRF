#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"

ActionInitialization::ActionInitialization(const DetectorConstruction* det)
  : fDet(det)
{
}

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction(fDet));
}

void ActionInitialization::Build() const
{
  SetUserAction(new RunAction(fDet));
  SetUserAction(new PrimaryGeneratorAction(fDet));
}