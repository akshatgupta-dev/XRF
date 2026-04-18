#include "SimulationMessenger.hh"

#include "primaryGenerator.hh"


SimulationMessenger::SimulationMessenger(SimulationConfig& config):fConfig(&config){


    fAddShieldLayerCmd = new G4UIcommand("/xrf/addShieldLayer", this);
    fAddShieldLayerCmd->SetGuidance("Add shield layer: material thickness unit gapbefore unit");

    auto* p1 = new G4UIparameter("material", 's', false);
    fAddShieldLayerCmd->SetParameter(p1);

    auto* p2 = new G4UIparameter("thickness", 'd', false);
    fAddShieldLayerCmd->SetParameter(p2);

    auto* p3 = new G4UIparameter("thicknessUnit", 's', false);
    fAddShieldLayerCmd->SetParameter(p3);

    auto* p4 = new G4UIparameter("gapBefore", 'd', false);
    fAddShieldLayerCmd->SetParameter(p4);

    auto* p5 = new G4UIparameter("gapUnit", 's', false);
    fAddShieldLayerCmd->SetParameter(p5);


    fMessenger=new G4GenericMessenger(this,"/xrf/","Simulation control commands");
    fMessenger->DeclareProperty("sampleMaterial",fConfig->sampleMaterial,"Set sample material");

    fMessenger->DeclareProperty("sampleMaterialIsCustom",fConfig->sampleMaterialIsCustom,"Set if the sample material is custom defined");


    fMessenger->DeclarePropertyWithUnit("sampleMaterialDensity",
                                        "g/cm3",
                                        fConfig->sampleMaterialDensity,
                                        "Set the density of the custom sample material");



    fMessenger->DeclarePropertyWithUnit("incidentAngle",
                                        "deg",
                                        fConfig->incidentAngle,
                                        "Set the incident angle of the primary particles");


    fMessenger->DeclarePropertyWithUnit("sourceDistance",
                                        "mm",
                                        fConfig->sourceDistance,
                                        "Set the distance between the source and the sample");

    fMessenger->DeclarePropertyWithUnit("detectorDistance",
                                        "mm",
                                        fConfig->detectorDistance,
                                        "Set the distance between the sample and the detector");

    fMessenger->DeclarePropertyWithUnit("takeoffAngle",
                                        "deg",
                                        fConfig->takeoffAngle,
                                        "Set the takeoff angle of the detector");

    fMessenger->DeclarePropertyWithUnit("detectorSpreadDeg",
                                        "deg",
                                        fConfig->detectorSpreadDeg,
                                        "Set the angular spread of the detector array in degrees");

    fMessenger->DeclarePropertyWithUnit("detectorStepDeg",
                                        "deg",
                                        fConfig->detectorStepDeg,
                                        "Set the angular step between adjacent detectors in degrees");



    fMessenger->DeclareProperty("totalEvents",fConfig->totalEvents,"Set the total number of events to simulate");
    fMessenger->DeclareProperty("chunkSize",fConfig->chunkSize,"Set the number of events to simulate in each chunk");

    fMessenger->DeclareProperty("setShieldEnabled",fConfig->shield.enable,"Enable or disable the shield");

    fMessenger->DeclareProperty("numberOfDetectorGroups",fConfig->numDetectorGroups,"Set the number of detector groups (multi-detectors) to construct");


    fMessenger->DeclarePropertyWithUnit("setShieldGap",
                                        "mm",
                                        fConfig->shield.detectorAndFirstLayerGap,
                                        "Set the gap between the detector and the first layer of the shield");

fMessenger->DeclarePropertyWithUnit("sampleSize",
                                    "mm",
                                    fConfig->sampleMaterialSize,
                                    "Set the sample size");

    fMessenger->DeclareMethod("clearShieldLayers",&SimulationMessenger::ClearShieldLayers,"Clear all shield layers");

    fMessenger->DeclareMethod("addMaterialComponent",&SimulationMessenger::AddMaterialComponent,"Add a material component to the custom sample material, usage: /xrf/addMaterialComponent element fraction");

    fMessenger->DeclareMethod("clearMaterialComponents",&SimulationMessenger::ClearMaterialComponents,"Clear all material components from the custom sample material");

    fMessenger->DeclareProperty("worldMaterial",fConfig->worldMaterial,"Set the material of the world volume");

    fMessenger->DeclareProperty("detectorType",fConfig->detectorType,"Set the type of the detector (e.g., Si, Ge)");
    fMessenger->DeclarePropertyWithUnit("detectorThickness","mm",fConfig->detectorthickness,"Set the thickness of the detector in mm");

}

void SimulationMessenger::SetSampleSize(const G4String& value){
    G4cout << "SetSampleSize raw value = [" << value << "]" << G4endl;

    std::istringstream iss(value);
    G4double x = 0.0, y = 0.0, z = 0.0;
    std::string unit;

    if (!(iss >> x >> y >> z >> unit)) {
        G4cerr << "Usage: /xrf/setSampleSize x y z unit" << G4endl;
        return;
    }

    G4double unitValue = G4UnitDefinition::GetValueOf(unit.c_str());
    fConfig->sampleMaterialSize = G4ThreeVector(x * unitValue, y * unitValue, z * unitValue);

    G4cout << "Sample size set to "
           << fConfig->sampleMaterialSize.x()/mm << " "
           << fConfig->sampleMaterialSize.y()/mm << " "
           << fConfig->sampleMaterialSize.z()/mm << " mm" << G4endl;
}

void SimulationMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == fAddShieldLayerCmd) {
        std::istringstream iss(newValue);

        std::string material;
        G4double thickness = 0.0, gapBefore = 0.0;
        std::string thicknessUnit, gapUnit;

        if (!(iss >> material >> thickness >> thicknessUnit >> gapBefore >> gapUnit)) {
            G4cerr << "Usage: /xrf/addShieldLayer material thickness unit gapbefore unit" << G4endl;
            return;
        }

        G4double thicknessValue = G4UnitDefinition::GetValueOf(thicknessUnit.c_str());
        G4double gapValue = G4UnitDefinition::GetValueOf(gapUnit.c_str());

        fConfig->shield.layers.emplace_back(
            material,
            thickness * thicknessValue,
            gapBefore * gapValue
        );

        return;
    }

    if (fMessenger) {
        fMessenger->SetNewValue(command, newValue);
    }
}
void SimulationMessenger::ClearShieldLayers(){
    fConfig->shield.layers.clear();
}

void SimulationMessenger::AddMaterialComponent(const G4String& value){
    std::istringstream iss(value);
    std::string element;
    G4double fraction = 0.0;

    if (!(iss >> element >> fraction)) {
        G4cerr << "Usage: /xrf/addMaterialComponent element fraction" << G4endl;
        return;
    }

    fConfig->sampleMaterialComponents.emplace_back(element, fraction);
}

void SimulationMessenger::ClearMaterialComponents(){
    fConfig->sampleMaterialComponents.clear();
}

SimulationMessenger::~SimulationMessenger() {
    delete fAddShieldLayerCmd;
    delete fMessenger;    
}

