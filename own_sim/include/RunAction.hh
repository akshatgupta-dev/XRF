#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include <string>
#include <vector>

class DetectorConstruction;
class SimulationConfig; // Added forward declaration
class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    // Updated constructor to take the config pointer
    explicit RunAction(const DetectorConstruction* det, const SimulationConfig* config);
    ~RunAction() override;

    static RunAction* Instance();

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run) override;

    void ResetAll();
    void ScorePhoton(G4int detId, G4double energy);
    void WriteCheckpoint(long long cumulativeEvents) const;

  private:
    static RunAction* fgInstance;

    const DetectorConstruction* fDet = nullptr;
    
    // Added member variable for the configuration
    const SimulationConfig* fConfig = nullptr;

    G4int fNBins = 2048;
    G4double fEmin = 0.0;
    G4double fEmax = 50.0*keV;
    G4double fBinWidth = 0.0;

    std::vector<std::vector<G4double>> fSpectra;
};

#endif