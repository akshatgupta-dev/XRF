#include <iostream>

#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "physicsList.hh"
#include "primaryGenerator.hh"
#include "detectorConstruction.hh"
#include "SimulationMessenger.hh"
#include "SimulationConfig.hh"
#include "multiDetector.hh"
#include "detectorConstruction.hh"
#include "G4NistManager.hh"
#include "SimulationMessenger.hh"
#include "actionInitialization.hh"
#include "runAction.hh"

int main(int argc,char **argv){

    G4UIExecutive *ui=nullptr;  
    #ifdef G4MULTITHREADED
        G4MTRunManager *manager=new G4MTRunManager;

    #else
        G4RunManager *manager=new G4RunManager;
    #endif
    manager->SetUserInitialization(new PhysicsList());

    SimulationConfig config;
    SimulationMessenger messenger(config);


    manager->SetUserInitialization(new DetectorConstruction(config));

    manager->SetUserInitialization(new ActionInitialization(config));


    G4VisManager *vismanger=new G4VisExecutive();   
    
    vismanger->Initialize();


        G4UImanager *uimanager=G4UImanager::GetUIpointer();

    if (argc == 1) {
        ui = new G4UIExecutive(argc, argv);
    }

    if (ui){
        
        uimanager->ApplyCommand("/det/useFilter true");

        G4int result=uimanager->ApplyCommand("/control/execute vis.mac");
        G4cout << "Result = " << result << G4endl;
        
        
        ui->SessionStart();

    }
    else{
        G4String command="/control/execute ";
        G4String fileName=argv[1];
        uimanager->ApplyCommand(command+fileName);
    }

    return 0;
}