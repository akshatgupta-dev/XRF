#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4GeneralParticleSource.hh"

class DetectorConstruction;
class SimulationConfig;
class G4Event;
class G4ParticleGun;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(const DetectorConstruction* det, SimulationConfig* config);
    ~PrimaryGeneratorAction() override;

    static PrimaryGeneratorAction* Instance();

    void GeneratePrimaries(G4Event* event) override;

    void SetBeamEnergy(G4double e);
    G4double GetBeamEnergy() const { return fBeamEnergy; }

  private:
    static PrimaryGeneratorAction* fgInstance;

    const DetectorConstruction* fDet = nullptr;
    SimulationConfig* fConfig = nullptr;
    G4ParticleGun* fGun = nullptr;
    G4double fBeamEnergy = 0.0;
    G4GeneralParticleSource* fparticlegun = nullptr;
};

#endif