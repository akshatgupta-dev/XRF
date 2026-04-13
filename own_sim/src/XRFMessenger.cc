#include "XRFMessenger.hh"
#include "SimulationConfig.hh"
#include "DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcommand.hh"
#include <sstream>
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4StateManager.hh"

// 1. Fixed constructor signature to include DetectorConstruction*
XRFMessenger::XRFMessenger(SimulationConfig* config,
                           DetectorConstruction* detector,
                           std::function<void()> runCallback)
  : G4UImessenger(),
    fConfig(config), 
    fDetector(detector), // <-- Added detector initialization
    fRunCallback(std::move(runCallback))
{
  fXrfDir = new G4UIdirectory("/xrf/");
  fXrfDir->SetGuidance("XRF simulation control commands.");

  fSetMaterialCmd = new G4UIcmdWithAString("/xrf/setMaterial", this);
  fSetMaterialCmd->SetGuidance("Set sample material, e.g. G4_Fe.");
  fSetMaterialCmd->SetParameterName("material", false);

  fComboSizesCmd = new G4UIcmdWithAString("/xrf/comboSizes", this);
  fComboSizesCmd->SetGuidance("Set automatic combo sizes, e.g. '1 3 5 9'");

  fAddComboCmd = new G4UIcmdWithAString("/xrf/addCombo", this);
  fAddComboCmd->SetGuidance("Add a custom combo: <name> <id1> <id2> ...");

  fClearCustomCombosCmd = new G4UIcmdWithoutParameter("/xrf/clearCustomCombos", this);
  fClearCustomCombosCmd->SetGuidance("Clear all custom detector combos");

  fSetBeamEnergyCmd = new G4UIcmdWithADoubleAndUnit("/xrf/setBeamEnergy", this);
  fSetBeamEnergyCmd->SetGuidance("Set beam energy.");
  fSetBeamEnergyCmd->SetParameterName("beamEnergy", false);
  fSetBeamEnergyCmd->SetUnitCategory("Energy");

  fSetTotalEventsCmd = new G4UIcmdWithAString("/xrf/setTotalEvents", this);
  fSetTotalEventsCmd->SetGuidance("Set total number of events.");
  fSetTotalEventsCmd->SetParameterName("totalEvents", false);

  fSetChunkSizeCmd = new G4UIcmdWithAnInteger("/xrf/setChunkSize", this);
  fSetChunkSizeCmd->SetGuidance("Set chunk size.");
  fSetChunkSizeCmd->SetParameterName("chunkSize", false);

  fSetIncidentAngleCmd = new G4UIcmdWithADoubleAndUnit("/xrf/setIncidentAngle", this);
  fSetIncidentAngleCmd->SetGuidance("Set incident angle.");
  fSetIncidentAngleCmd->SetParameterName("incidentAngle", false);
  fSetIncidentAngleCmd->SetUnitCategory("Angle");

  fSetSourceDistanceCmd = new G4UIcmdWithADoubleAndUnit("/xrf/setSourceDistance", this);
  fSetSourceDistanceCmd->SetGuidance("Set source distance.");
  fSetSourceDistanceCmd->SetParameterName("sourceDistance", false);
  fSetSourceDistanceCmd->SetUnitCategory("Length");

  // 2. Added the missing geometry commands that are used in SetNewValue()
  fSetDetectorDistanceCmd = new G4UIcmdWithADoubleAndUnit("/xrf/setDetectorDistance", this);
  fSetDetectorDistanceCmd->SetGuidance("Set the distance to the detector.");
  fSetDetectorDistanceCmd->SetParameterName("distance", false);
  fSetDetectorDistanceCmd->SetUnitCategory("Length");

  fSetNominalTakeoffCmd = new G4UIcmdWithADoubleAndUnit("/xrf/setNominalTakeoff", this);
  fSetNominalTakeoffCmd->SetGuidance("Set nominal takeoff angle.");
  fSetNominalTakeoffCmd->SetParameterName("angle", false);
  fSetNominalTakeoffCmd->SetUnitCategory("Angle");

  fSetSpreadCmd = new G4UIcmdWithADoubleAndUnit("/xrf/setSpread", this);
  fSetSpreadCmd->SetGuidance("Set detector spread angle.");
  fSetSpreadCmd->SetParameterName("angle", false);
  fSetSpreadCmd->SetUnitCategory("Angle");

  fSetStepCmd = new G4UIcmdWithADoubleAndUnit("/xrf/setStep", this);
  fSetStepCmd->SetGuidance("Set detector step angle.");
  fSetStepCmd->SetParameterName("angle", false);
  fSetStepCmd->SetUnitCategory("Angle");

  fSetShieldEnabledCmd = new G4UIcmdWithABool("/xrf/setShieldEnabled", this);
  fSetShieldEnabledCmd->SetGuidance("Enable or disable detector shield.");
  fSetShieldEnabledCmd->SetParameterName("enabled", false);

  fSetShieldDetectorGapCmd = new G4UIcmdWithADoubleAndUnit("/xrf/setShieldDetectorGap", this);
  fSetShieldDetectorGapCmd->SetGuidance("Set gap from detector to first shield layer.");
  fSetShieldDetectorGapCmd->SetParameterName("gap", false);
  fSetShieldDetectorGapCmd->SetUnitCategory("Length");

  fAddShieldLayerCmd = new G4UIcmdWithAString("/xrf/addShieldLayer", this);
  fAddShieldLayerCmd->SetGuidance("Add shield layer: <material> <thickness> <unit> [gapBefore] [gapUnit]");

  fClearShieldLayersCmd = new G4UIcmdWithoutParameter("/xrf/clearShieldLayers", this);
  fClearShieldLayersCmd->SetGuidance("Remove all shield layers.");

  fRunCmd = new G4UIcmdWithoutParameter("/xrf/run", this);
  fRunCmd->SetGuidance("Run the checkpointed XRF job with current config.");
}

XRFMessenger::~XRFMessenger()
{
  delete fRunCmd;
  
  // 3. Added missing deletions for the new commands to prevent memory leaks
  delete fSetSourceDistanceCmd;
  delete fSetIncidentAngleCmd;
  delete fSetStepCmd;
  delete fSetSpreadCmd;
  delete fSetNominalTakeoffCmd;
  delete fSetDetectorDistanceCmd;
  delete fComboSizesCmd;
  delete fClearShieldLayersCmd;
  delete fAddShieldLayerCmd;
  delete fSetShieldDetectorGapCmd;
  delete fSetShieldEnabledCmd;
  delete fAddComboCmd;
  delete fClearCustomCombosCmd;
  delete fSetChunkSizeCmd;
  delete fSetTotalEventsCmd;
  delete fSetBeamEnergyCmd;
  delete fSetMaterialCmd;
  delete fXrfDir;
}

void XRFMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  auto refreshGeometry = []() {
    auto* rm = G4RunManager::GetRunManager();
    if (rm && G4StateManager::GetStateManager()->GetCurrentState() != G4State_PreInit) {
      rm->ReinitializeGeometry(true);
    }
  };

  if (command == fSetMaterialCmd) {
    fDetector->SetSampleMaterial(newValue);
  }
  else if (command == fSetBeamEnergyCmd) {
    fConfig->beamEnergy = fSetBeamEnergyCmd->GetNewDoubleValue(newValue);
  }
  else if (command == fSetTotalEventsCmd) {
    fConfig->totalEvents = std::stoll(newValue);
  }
  else if (command == fSetChunkSizeCmd) {
    fConfig->chunkSize = fSetChunkSizeCmd->GetNewIntValue(newValue);
  }
  else if (command == fSetDetectorDistanceCmd) {
    const auto v = fSetDetectorDistanceCmd->GetNewDoubleValue(newValue);
    fDetector->SetDetectorDistance(v);
  }
  else if (command == fSetIncidentAngleCmd) {
    const auto v = fSetIncidentAngleCmd->GetNewDoubleValue(newValue);
    fDetector->SetIncidentAngleDeg(v / deg);
  }
  else if (command == fComboSizesCmd) {
    fConfig->comboSizes.clear();

    std::istringstream iss(newValue);
    G4int n;
    while (iss >> n) {
      if (n > 0) {
        fConfig->comboSizes.push_back(n);
      }
    }
  }
  else if (command == fAddComboCmd) {
    std::istringstream iss(newValue);

    std::string name;
    iss >> name;

    if (!name.empty()) {
      NamedComboConfig combo;
      combo.name = name;

      G4int detId;
      while (iss >> detId) {
        combo.detIds.push_back(detId);
      }

      if (!combo.detIds.empty()) {
        fConfig->customCombos.push_back(combo);
      }
    }
  }
  else if (command == fClearCustomCombosCmd) {
    fConfig->customCombos.clear();
  }
  else if (command == fSetSourceDistanceCmd) {
    const auto v = fSetSourceDistanceCmd->GetNewDoubleValue(newValue);
    fDetector->SetSourceDistance(v);
  }
  else if (command == fSetNominalTakeoffCmd) {
    const auto v = fSetNominalTakeoffCmd->GetNewDoubleValue(newValue);
    fDetector->SetNominalTakeoffDeg(v / deg);
  }
  else if (command == fSetSpreadCmd) {
    const auto v = fSetSpreadCmd->GetNewDoubleValue(newValue);
    fDetector->SetDetectorSpreadDeg(v / deg);
  }
  else if (command == fSetStepCmd) {
    const auto v = fSetStepCmd->GetNewDoubleValue(newValue);
    fDetector->SetDetectorStepDeg(v / deg);
  }
  else if (command == fSetShieldEnabledCmd) {
    fDetector->SetShieldEnabled(fSetShieldEnabledCmd->GetNewBoolValue(newValue));
    refreshGeometry();
  }
  else if (command == fSetShieldDetectorGapCmd) {
    const auto v = fSetShieldDetectorGapCmd->GetNewDoubleValue(newValue);
    fDetector->SetShieldDetectorGap(v);
    refreshGeometry();
  }
  else if (command == fClearShieldLayersCmd) {
    fDetector->ClearShieldLayers();
    refreshGeometry();
  }
  else if (command == fAddShieldLayerCmd) {
    std::istringstream iss(newValue);

    std::string material;
    G4double thickValue = 0.0;
    std::string thickUnit;
    G4double gapValue = 0.0;
    std::string gapUnit;

    if (!(iss >> material >> thickValue >> thickUnit)) {
      G4cerr << "Usage: /xrf/addShieldLayer G4_Pb 2.0 mm [gapBefore gapUnit]" << G4endl;
      return;
    }

    const G4double thickness = thickValue * G4UIcommand::ValueOf(thickUnit.c_str());

    G4double gapBefore = 0.0;
    if (iss >> gapValue >> gapUnit) {
      gapBefore = gapValue * G4UIcommand::ValueOf(gapUnit.c_str());
    }

    fDetector->AddShieldLayer(material, thickness, gapBefore);
    refreshGeometry();
  }
  else if (command == fRunCmd) {
    if (fRunCallback) {
      fRunCallback();
    }
  }
}