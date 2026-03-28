#ifndef PMPRIMARYGENERATOR_HH
#define PMPRIMARYGENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

// Forward declarations
class DetectorConstruction;
class SimulationConfig;

class PMPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
    public:
        // Updated constructor to take the detector and config pointers
        explicit PMPrimaryGenerator(const DetectorConstruction* det, SimulationConfig* config);
        ~PMPrimaryGenerator() override;

        void GeneratePrimaries(G4Event* event) override;

    private:
        G4ParticleGun* fParticleGun = nullptr;
        
        // Added member variables
        const DetectorConstruction* fDetector = nullptr;
        SimulationConfig* fConfig = nullptr;
};

#endif