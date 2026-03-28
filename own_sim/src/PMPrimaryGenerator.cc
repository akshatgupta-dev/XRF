#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "SimulationConfig.hh"

#include "G4Event.hh"
#include "G4Gamma.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction* PrimaryGeneratorAction::fgInstance = nullptr;

PrimaryGeneratorAction::PrimaryGeneratorAction(const DetectorConstruction* det,
                                               SimulationConfig* config)
  : fDet(det), fConfig(config)
{
  fgInstance = this;

  fGun = new G4ParticleGun(1);
  fGun->SetParticleDefinition(G4Gamma::Gamma());
  SetBeamEnergy(fConfig->beamEnergy);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  if (fgInstance == this) {
    fgInstance = nullptr;
  }
  delete fGun;
}

PrimaryGeneratorAction* PrimaryGeneratorAction::Instance()
{
  return fgInstance;
}

void PrimaryGeneratorAction::SetBeamEnergy(G4double e)
{
  fConfig->beamEnergy = e;
  fBeamEnergy = e;
  fGun->SetParticleEnergy(fBeamEnergy);
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  fGun->SetParticlePosition(fDet->GetSourcePosition());
  fGun->SetParticleMomentumDirection(fDet->GetSourceDirection());
  fGun->GeneratePrimaryVertex(event);
}