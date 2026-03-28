#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

#include <string>
#include <vector>

class G4LogicalVolume;
class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    // Geometry config
    void SetSampleMaterial(const std::string& name);
    void SetIncidentAngleDeg(G4double val);
    void SetSourceDistance(G4double val);
    void SetDetectorDistance(G4double val);
    void SetNominalTakeoffDeg(G4double val);
    void SetDetectorSpreadDeg(G4double val);
    void SetDetectorStepDeg(G4double val);

    // Accessors
    const std::string& GetSampleMaterial() const { return fSampleMaterial; }
    G4double GetIncidentAngleDeg() const { return fIncidentAngleDeg; }
    G4double GetSourceDistance() const { return fSourceDistance; }
    G4double GetDetectorDistance() const { return fDetectorDistance; }
    G4double GetNominalTakeoffDeg() const { return fNominalTakeoffDeg; }
    G4double GetDetectorSpreadDeg() const { return fDetectorSpreadDeg; }
    G4double GetDetectorStepDeg() const { return fDetectorStepDeg; }

    const std::vector<G4double>& GetDetectorAnglesDeg() const { return fDetectorAnglesDeg; }
    G4int GetNumberOfDetectors() const { return static_cast<G4int>(fDetectorAnglesDeg.size()); }

    G4ThreeVector GetSourcePosition() const;
    G4ThreeVector GetSourceDirection() const;

  private:
    void UpdateAngleList();

    std::string fSampleMaterial = "G4_SILICON_DIOXIDE";

    // Angles measured from sample plane
    G4double fIncidentAngleDeg   = 45.0;
    G4double fNominalTakeoffDeg  = 45.0;
    G4double fDetectorSpreadDeg  = 4.0;
    G4double fDetectorStepDeg    = 1.0;

    G4double fSourceDistance     = 50.0;  // mm
    G4double fDetectorDistance   = 50.0;  // mm

    std::vector<G4double> fDetectorAnglesDeg;

    G4LogicalVolume* fDetectorLV = nullptr;
};

#endif