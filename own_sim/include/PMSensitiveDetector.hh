#ifndef PMSENSITIVEDETECTOR_HH
#define PMSENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <map>
#include <unordered_set>

class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

class PMSensitiveDetector : public G4VSensitiveDetector
{
public:
    PMSensitiveDetector(G4String);
    ~PMSensitiveDetector() override;

    void Initialize(G4HCofThisEvent*) override;
    void EndOfEvent(G4HCofThisEvent*) override;
    G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;

private:
    G4double fTotalDetectedEnergy;
    std::map<G4int, G4int> fFluorescenceCountsByBin;
    std::unordered_set<G4int> fCountedTrackIds;

    static constexpr G4double kBinWidthKeV = 0.1;
};

#endif