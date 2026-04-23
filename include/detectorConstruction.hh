#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH


#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "SimulationConfig.hh"
#include "globals.hh"
#include "G4GenericMessenger.hh"
#include "multiDetector.hh"
#include "G4VPhysicalVolume.hh"
#include "detectorElement.hh"
#include "G4Box.hh"
#include "G4Element.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4RunManager.hh"

#include <algorithm>
#include <string>
#include <vector>



struct DetectorMeta {
    G4int detId;
    G4String material;
    G4double samplewidth, sampleheight, samplethickness;
    G4double incidentAngle;
    G4double sourceDistance;
    G4double detectorDistance;
    G4double takeoffAngle;
    G4double detectorx, detectory, detectorz;
    G4double width, thickness, height;
    G4String worldMat;
    G4String detectorType;

    DetectorMeta(
        G4int id,
        G4String mat,
        G4double sw, G4double sh, G4double st,
        G4double incAngle,
        G4double srcDist,
        G4double detDist,
        G4double takeoff,
        G4double detX, G4double detY, G4double detZ,
        G4double w, G4double t, G4double h,
        G4String worldMaterial,
        G4String detType
    ) : detId(id), material(mat), samplewidth(sw), sampleheight(sh), samplethickness(st),
        incidentAngle(incAngle), sourceDistance(srcDist), detectorDistance(detDist), takeoffAngle(takeoff),
        detectorx(detX), detectory(detY), detectorz(detZ),
        width(w), thickness(t), height(h),
        worldMat(worldMaterial),
        detectorType(detType) {}
};

struct VirtualDetectorMeta{

    G4VThreeVector center;
    G4double width, height, thickness;
    G4double thetadeg;
    G4double incidentAngle;
    std::vector <G4double> copyNumbersVec;


}

class DetectorConstruction:public G4VUserDetectorConstruction{

    public:
        DetectorConstruction(SimulationConfig& config);
        ~DetectorConstruction() override;
        G4VPhysicalVolume* Construct() override;
        SimulationConfig *fConfig{nullptr};
        std::vector<MultiDetectorConstruction> allDetectors ; 
        G4LogicalVolume *fDetectorLV{nullptr};
        void ConstructSDandField() override;
        const std::vector<DetectorMeta>& GetDetectorMetadata() const {
            return detectorMetadata;
        }

        void sortDetectorsRow(std::vector<DetectorElement>& detectors);

        


    
    private:
        std::vector<DetectorMeta> detectorMetadata;






};
#endif