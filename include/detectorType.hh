#ifndef DETECTORTYPE_HH
#define DETECTORTYPE_HH


#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"
#include <map>
#include "G4AnalysisManager.hh"
#include "SimulationConfig.hh"
#include "detectorElement.hh"

class SensitiveDetector:public G4VSensitiveDetector{

public:
    SensitiveDetector(G4String name,SimulationConfig &config,std::vector<VirtualDetector>& virtualDetectors);
    ~SensitiveDetector();
private:
    G4double fTotalEnergyDeposited;

    SimulationConfig* fConfig{nullptr};

    virtual void Initialize (G4HCofThisEvent *)override;
    virtual void EndOfEvent(G4HCofThisEvent*)override;
    virtual G4bool ProcessHits(G4Step*,G4TouchableHistory*);
    std::map<G4int, G4double> fDetectorEnergyMap;
    std::map<G4int, G4double> fEnergyMap;

    std::vector<VirtualDetector> allvirtualDetectors;

};

#endif 