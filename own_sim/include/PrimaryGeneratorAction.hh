#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class DetectorConstruction;
class G4Event;
class G4ParticleGun;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    explicit PrimaryGeneratorAction(const DetectorConstruction* det);
    ~PrimaryGeneratorAction() override;

    static PrimaryGeneratorAction* Instance();

    void GeneratePrimaries(G4Event* event) override;

    void SetBeamEnergy(G4double e);
    G4double GetBeamEnergy() const { return fBeamEnergy; }

  private:
    static PrimaryGeneratorAction* fgInstance;

    const DetectorConstruction* fDet = nullptr;
    G4ParticleGun* fGun = nullptr;
    G4double fBeamEnergy = 20.0;
};

#endif