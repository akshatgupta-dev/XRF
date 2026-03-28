#ifndef DetectorSD_h
#define DetectorSD_h 1

#include "G4VSensitiveDetector.hh"

class G4Step;
class G4TouchableHistory;

class DetectorSD : public G4VSensitiveDetector
{
  public:
    explicit DetectorSD(const G4String& name);
    ~DetectorSD() override = default;

    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
};

#endif