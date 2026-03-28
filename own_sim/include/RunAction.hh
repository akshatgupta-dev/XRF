#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include <string>
#include <vector>

class DetectorConstruction;
class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    explicit RunAction(const DetectorConstruction* det);
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

    G4int fNBins = 2048;
    G4double fEmin = 0.0;
    G4double fEmax = 50.0*keV;
    G4double fBinWidth = 0.0;

    std::vector<std::vector<G4double>> fSpectra;
};

#endif