#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "SimulationConfig.hh"

#include "G4Event.hh"
#include "G4Gamma.hh"
#include "G4GeneralParticleSource.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction* PrimaryGeneratorAction::fgInstance = nullptr;

PrimaryGeneratorAction::PrimaryGeneratorAction(const DetectorConstruction* det,
                                               SimulationConfig* config)
  : fDet(det), fConfig(config)
{
  fgInstance = this;

  fparticlegun = new G4GeneralParticleSource();
  auto* source = fparticlegun->GetCurrentSource();
  source->SetParticleDefinition(G4Gamma::Gamma());
  source->GetPosDist()->SetPosDisType("Point");
  source->GetPosDist()->SetCentreCoords(fDet->GetSourcePosition());
  source->GetAngDist()->SetAngDistType("beam");
  source->GetAngDist()->SetParticleMomentumDirection(fDet->GetSourceDirection());
  source->GetEneDist()->SetEnergyDisType("Mono");
  SetBeamEnergy(fConfig->beamEnergy);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  if (fgInstance == this) {
    fgInstance = nullptr;
  }
  delete fparticlegun;
}

PrimaryGeneratorAction* PrimaryGeneratorAction::Instance()
{
  return fgInstance;
}

void PrimaryGeneratorAction::SetBeamEnergy(G4double e)
{
  fConfig->beamEnergy = e;
  fBeamEnergy = e;
  if (fparticlegun) {
    auto* source = fparticlegun->GetCurrentSource();
    source->GetEneDist()->SetEnergyDisType("Mono");
    source->GetEneDist()->SetMonoEnergy(fBeamEnergy);
  }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  if (fparticlegun) {
    auto* source = fparticlegun->GetCurrentSource();
    source->GetPosDist()->SetCentreCoords(fDet->GetSourcePosition());
    source->GetAngDist()->SetParticleMomentumDirection(fDet->GetSourceDirection());
    fparticlegun->GeneratePrimaryVertex(event);
  }
}