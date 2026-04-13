#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "SimulationConfig.hh"
#include "G4GeneralParticleSource.hh"
#include "G4Event.hh"
#include "G4Gamma.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction* PrimaryGeneratorAction::fgInstance = nullptr;

PrimaryGeneratorAction::PrimaryGeneratorAction(const DetectorConstruction* det,
                                               SimulationConfig* config)
  : fDet(det), fConfig(config)
{
   fparticlegun=new G4GeneralParticleSource();
  // fgInstance = this;

  // fGun = new G4ParticleGun(1);
  // fGun->SetParticleDefinition(G4Gamma::Gamma());
  // SetBeamEnergy(fConfig->beamEnergy);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  // if (fgInstance == this) {
  //   fgInstance = nullptr;
  // }
  // delete fparticlegun;
  delete fparticlegun;
}

// PrimaryGeneratorAction* PrimaryGeneratorAction::Instance()
// {
// return fgInstance;
// }

// void PrimaryGeneratorAction::SetBeamEnergy(G4double e)
// {
//   fConfig->beamEnergy = e;
//   fBeamEnergy = e;
//   fparticlegun->SetParticleEnergy(fBeamEnergy);
// }

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
// fparticlegun->SetParticlePosition(fDet->GetSourcePosition());
// fparticlegun->SetParticleMomentumDirection(fDet->GetSourceDirection());
// fparticlegun->GeneratePrimaryVertex(event);
fparticlegun->GeneratePrimaryVertex(event);

}