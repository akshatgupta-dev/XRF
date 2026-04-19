#ifndef SIMULATIONCONFIG_HH
#define SIMULATIONCONFIG_HH

#include <string>
#include <vector>
#include  "globals.hh"
#include "G4SystemOfUnits.hh"


class MaterialComponent{

    public:
        G4String element;
        G4double fraction;
        MaterialComponent(const std::string& name, G4double frac):element(name),fraction(frac){}

};

class ShieldLayerConfig{
    public:
        std::string material;
        G4double thickness;
        G4double gapbefore;

        ShieldLayerConfig(std::string &material_name,G4double t,G4double gap):material(material_name),thickness(t),gapbefore(gap){}
};
class ShieldConfig{

    public:
        G4bool enable;
        G4double detectorAndFirstLayerGap;
        std::vector<ShieldLayerConfig> layers;

};


class SimulationConfig{

    public:
        G4String sampleMaterial;
        G4bool sampleMaterialIsCustom = false;
        G4double sampleMaterialDensity = 0.0;

        std::vector<MaterialComponent> sampleMaterialComponents;

        G4ThreeVector sampleMaterialSize = G4ThreeVector(0,0,0);

        G4double incidentAngle = 0.0;
        G4double sourceDistance = 0.0 * mm;
        G4double detectorDistance = 0.0 * mm;
        G4double takeoffAngle = 0.0;
        G4double detectorSpreadDeg = 0.0;
        G4double detectorStepDeg = 0.0;
        G4double detectorthickness = 0.0 * mm;

        long long totalEvents = 0;
        int chunkSize = 0;

        G4String worldMaterial = "G4_AIR";

        G4String detectorType;

        G4int numDetectorGroups;



        ShieldConfig shield;

};

#endif