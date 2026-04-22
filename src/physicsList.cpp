#include "physicsList.hh"


PhysicsList::PhysicsList(){
    RegisterPhysics(new G4EmLivermorePhysics());
    auto parameters=G4EmParameters::Instance();
    parameters->SetFluo(true);

    parameters->SetAuger(true);

}

PhysicsList::~PhysicsList(){


}