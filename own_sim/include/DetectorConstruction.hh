#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "SimulationConfig.hh"
#include "globals.hh"
#include "G4GenericMessenger.hh"


#include <string>
#include <vector>

struct DetectorElement {
    G4int id = -1;
    G4ThreeVector center;
    G4double thetaDeg = 0.0;
    G4double phiDeg = 0.0;
    G4double width = 0.0;
    G4double height = 0.0;
    G4bool enabled = true;
};

class G4LogicalVolume;
class G4VPhysicalVolume;
class SimulationConfig;

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    explicit DetectorConstruction(SimulationConfig* config);
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    void SetSampleMaterial(const std::string& name);
    void SetIncidentAngleDeg(G4double val);
    void SetSourceDistance(G4double val);
    void SetDetectorDistance(G4double val);
    void SetNominalTakeoffDeg(G4double val);
    void SetDetectorSpreadDeg(G4double val);
    void SetDetectorStepDeg(G4double val);



    void SetShieldEnabled(G4bool val);
void SetShieldDetectorGap(G4double val);
void ClearShieldLayers();
void AddShieldLayer(const std::string& material,
                    G4double thickness,
                    G4double gapBefore = 0.0);


    const std::string& GetSampleMaterial() const { return fConfig->sampleMaterial; }
    G4double GetIncidentAngleDeg() const { return fConfig->incidentAngleDeg; }
    G4double GetSourceDistance() const { return fConfig->sourceDistance; }
    G4double GetDetectorDistance() const { return fConfig->detectorDistance; }
    G4double GetNominalTakeoffDeg() const { return fConfig->nominalTakeoffDeg; }
    G4double GetDetectorSpreadDeg() const { return fConfig->detectorSpreadDeg; }
    G4double GetDetectorStepDeg() const { return fConfig->detectorStepDeg; }

    const std::vector<DetectorElement>& GetDetectorElements() const { return fDetectorElements; }

    const DetectorElement& GetDetectorElement(G4int id) const {
        return fDetectorElements.at(id);
    }

    G4int GetNumberOfDetectors() const {
        return static_cast<G4int>(fDetectorElements.size());
    }

    G4ThreeVector GetSourcePosition() const;
    G4ThreeVector GetSourceDirection() const;

    // NEW: force metadata rebuild from current config
    void RefreshDetectorLayout();

  private:
    void RebuildDetectorElements();
    G4double ComputeThetaDeg(const G4ThreeVector& p) const;
    G4double ComputePhiDeg(const G4ThreeVector& p) const;

    G4double GetDetectorCapRadius() const;
    G4double GetDetectorPixelSize() const;
    void SetWorldMaterial(const G4String &name);

    SimulationConfig* fConfig = nullptr;
    std::vector<DetectorElement> fDetectorElements;
    G4LogicalVolume* fDetectorLV = nullptr;
    G4GenericMessenger *fMessenger=nullptr;
    G4String fworldmaterialname="G4_Air";

};

#endif