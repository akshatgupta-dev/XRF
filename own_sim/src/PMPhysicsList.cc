#include "PMPhysicsList.hh"

PMPhysicsList::PMPhysicsList()
{
    // EM Physics
    RegisterPhysics(new G4EmLivermorePhysics());
}

PMPhysicsList::~PMPhysicsList()
{
    
}