#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class DetectorConstruction;
class SimulationConfig; // Added forward declaration

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    // Updated constructor to take the config pointer
    explicit ActionInitialization(const DetectorConstruction* det, SimulationConfig* config);
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;

  private:
    const DetectorConstruction* fDet = nullptr;
    
    // Added member variable for the configuration
    SimulationConfig* fConfig = nullptr;
};

#endif