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
struct RecordingInfo
{
    G4int detId;
    G4double energy;
    G4int trackId;
    G4double arivedTime;

    RecordingInfo()
        : detId(-1), energy(0.0), trackId(-1), arivedTime(0.0) {}

    RecordingInfo(G4int id, G4double edep, G4int tid, G4double time)
        : detId(id), energy(edep), trackId(tid), arivedTime(time) {}
};

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
    std::map<G4int,std::map<G4int,RecordingInfo>> recordingInfos;

};

#endif 