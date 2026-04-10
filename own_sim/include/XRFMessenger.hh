#ifndef XRFMessenger_h
#define XRFMessenger_h 1
#include "G4RunManager.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UImessenger.hh"
#include "globals.hh"

#include <functional>

class SimulationConfig;
class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithoutParameter;

class XRFMessenger : public G4UImessenger
{
  public:
    XRFMessenger(SimulationConfig* config,
                 DetectorConstruction* detector,
                 std::function<void()> runCallback);
    ~XRFMessenger() override;

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

  private:
    SimulationConfig* fConfig = nullptr;
    DetectorConstruction* fDetector = nullptr;
    std::function<void()> fRunCallback;

    G4UIdirectory* fXrfDir = nullptr;

    G4UIcmdWithAString* fSetMaterialCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetBeamEnergyCmd = nullptr;
    G4UIcmdWithAString* fSetTotalEventsCmd = nullptr;
    G4UIcmdWithAnInteger* fSetChunkSizeCmd = nullptr;

    G4UIcmdWithADoubleAndUnit* fSetDetectorDistanceCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetNominalTakeoffCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetSpreadCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetIncidentAngleCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetSourceDistanceCmd = nullptr;

    G4UIcmdWithADoubleAndUnit* fSetStepCmd = nullptr;

    G4UIcmdWithoutParameter* fRunCmd = nullptr;

    // --- Added combo commands ---
    G4UIcmdWithAString* fComboSizesCmd = nullptr;
    G4UIcmdWithAString* fAddComboCmd = nullptr;
    G4UIcmdWithoutParameter* fClearCustomCombosCmd = nullptr;
};

#endif