#include "physicsList.hh"
#include "G4EmParameters.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4ios.hh"

PhysicsList::PhysicsList()
{
    RegisterPhysics(new G4EmLivermorePhysics());

    auto* parameters = G4EmParameters::Instance();
    parameters->SetFluo(true);
    parameters->SetAuger(true);
    parameters->SetPixe(true);
    parameters->SetDeexcitationIgnoreCut(true);
    parameters->SetVerbose(0);

    G4cout << "PhysicsList: Fluo/Auger/PIXE/IgnoreCut set in C++" << G4endl;
}

PhysicsList::~PhysicsList() {}