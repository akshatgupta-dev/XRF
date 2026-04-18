#ifndef SIMULATIONMESSAGENGER_HH
#define SIMULATIONMESSAGENGER_HH

#include "G4UImessenger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "SimulationConfig.hh"
#include "SimulationConfig.hh"
#include "G4GenericMessenger.hh"
#include "G4UnitsTable.hh"
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"


class SimulationMessenger:public G4UImessenger
{
    public:

    explicit  SimulationMessenger(SimulationConfig& config);
    ~SimulationMessenger();
void SetSampleSize(const G4String& value);
void AddShieldLayer(const G4String& value);
void ClearShieldLayers();
    void SetNewValue(G4UIcommand* command, G4String newValue) override;

void AddMaterialComponent(const G4String& value);
void ClearMaterialComponents();

void CalculateSourcePositionAndDirection();
    private:
        SimulationConfig * fConfig{nullptr};
        G4GenericMessenger *fMessenger{nullptr};
        G4UIcommand* fAddShieldLayerCmd = nullptr;

};




#endif 