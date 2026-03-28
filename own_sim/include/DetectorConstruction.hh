#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "SimulationConfig.hh"
#include "globals.hh"

#include <string>
#include <vector>

class G4LogicalVolume;
class G4VPhysicalVolume;
class SimulationConfig; // Forward declaration added

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    // Updated constructor
    explicit DetectorConstruction(SimulationConfig* config);
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

    // Accessors updated to read from fConfig
    const std::string& GetSampleMaterial() const { return fConfig->sampleMaterial; }
    G4double GetIncidentAngleDeg() const { return fConfig->incidentAngleDeg; }
    G4double GetSourceDistance() const { return fConfig->sourceDistance; }
    G4double GetDetectorDistance() const { return fConfig->detectorDistance; }
    G4double GetNominalTakeoffDeg() const { return fConfig->nominalTakeoffDeg; }
    G4double GetDetectorSpreadDeg() const { return fConfig->detectorSpreadDeg; }
    G4double GetDetectorStepDeg() const { return fConfig->detectorStepDeg; }

    const std::vector<G4double>& GetDetectorAnglesDeg() const { return fDetectorAnglesDeg; }
    G4int GetNumberOfDetectors() const { return static_cast<G4int>(fDetectorAnglesDeg.size()); }

    G4ThreeVector GetSourcePosition() const;
    G4ThreeVector GetSourceDirection() const;

  private:
    void UpdateAngleList();

    // Config pointer added; hardcoded defaults removed
    SimulationConfig* fConfig = nullptr;

    std::vector<G4double> fDetectorAnglesDeg;
    G4LogicalVolume* fDetectorLV = nullptr;
};

#endif