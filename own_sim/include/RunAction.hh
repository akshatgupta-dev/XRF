#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"

#include <string>
#include <map>
#include <vector>

#include "G4Timer.hh"
#include <chrono>

class DetectorConstruction;
class SimulationConfig;
class G4Run;

struct DetectorCombo {
    std::string name;
    std::vector<G4int> detIds;
};

class RunAction : public G4UserRunAction
{
  public:
    explicit RunAction(const DetectorConstruction* det, const SimulationConfig* config);
    ~RunAction() override;

    static RunAction* Instance();

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction(const G4Run* run) override;

    void ResetAll();
    void ScorePhoton(G4int detId, G4double energy);
    void WriteCheckpoint(long long cumulativeEvents) const;
    void ResetShieldLayerDebug();
    void RecordShieldLayerEntry(G4int layer, G4double edep);
    void SyncWithDetectorLayout();
    void FinalizeRun(long long cumulativeEvents) const;

    // NEW: ROOT helper methods
    std::string BuildRootFilename(long long cumulativeEvents) const;
    void BookRootOutput();
    void OpenRootFile();
    void CloseRootFile() const;

  private:
    static RunAction* fgInstance;

    const DetectorConstruction* fDet = nullptr;
    const SimulationConfig* fConfig = nullptr;

    G4int fNBins = 2048;
    G4double fEmin = 0.0;
    G4double fEmax = 50.0 * keV;
    G4double fBinWidth = 0.0;

    std::vector<std::vector<G4double>> fSpectra;
    std::vector<DetectorCombo> fCombos;

    void RebuildCombos();
    std::vector<G4double> SumSpectrum(const std::vector<G4int>& detIds) const;
    void WriteComboCheckpoint(long long cumulativeEvents) const;
    std::string BuildComboFilename(long long cumulativeEvents) const;

    std::map<G4int, G4long> fShieldLayerEntries;
    std::map<G4int, G4double> fShieldLayerEdep;

    // NEW: track whether ROOT file is open
    mutable G4bool fRootOpen = false;
    mutable G4Timer fCpuTimer;
    std::chrono::steady_clock::time_point fWallStart;
};

#endif