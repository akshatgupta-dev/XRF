#ifndef DetectorSD_h
#define DetectorSD_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class G4Step;
class G4TouchableHistory;
class G4HCofThisEvent;

class DetectorSD : public G4VSensitiveDetector
{
  public:
    explicit DetectorSD(const G4String& name);
    ~DetectorSD() override = default;

    void Initialize(G4HCofThisEvent* hce) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    void EndOfEvent(G4HCofThisEvent* hce) override;

  private:
    G4double fTotalEnergyDeposited = 0.0;
};

#endif
