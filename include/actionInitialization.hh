#ifndef ACTIONINITIALIZATION_HH
#define ACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"
#include "primaryGenerator.hh"
#include "runAction.hh"
#include "SimulationConfig.hh"

class ActionInitialization:public G4VUserActionInitialization{

    public:
        ActionInitialization(SimulationConfig& config):fConfig(&config){};
        ~ActionInitialization() override;
        void Build() const override;
        void BuildForMaster() const override;

        SimulationConfig *fConfig{nullptr};


};

#endif