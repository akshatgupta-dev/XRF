#include "physicsList.hh"


PhysicsList::PhysicsList(){
    auto parameters=G4EmParameters::Instance();

    parameters->SetFluo(true);
    parameters->SetAuger(true);
    parameters->SetPixe(true);
parameters->SetFluoDirectory(fluoANSTO);
    parameters->SetPIXECrossSectionModel("ANSTO");

    parameters->SetDeexcitationIgnoreCut(true); 

    RegisterPhysics(new G4EmLivermorePhysics());



}

PhysicsList::~PhysicsList(){


}