#ifndef PMSENSITIVEDETECTOR_HH
#define PMSENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

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
    G4double fTotalEnergyDeposited;
};

#endif