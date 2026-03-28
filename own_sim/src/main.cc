#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"

#include "SimulationConfig.hh" // <-- Added Config header

#include "FTFP_BERT.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmParameters.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4ios.hh"

#include <algorithm>
#include <string>
#include <vector>

namespace
{
  void RunOneCheckpointedJob(G4RunManager* runManager,
                             RunAction* runAction,
                             long long totalEvents,
                             int chunkSize)
  {
    runAction->ResetAll();

    long long processed = 0;
    while (processed < totalEvents) {
      const int thisChunk =
          static_cast<int>(std::min<long long>(chunkSize, totalEvents - processed));

      G4cout << "Starting chunk with " << thisChunk
             << " events. Processed so far = " << processed << G4endl;

      runManager->BeamOn(thisChunk);
      processed += thisChunk;

      G4cout << "Finished chunk. Total processed = "
             << processed << G4endl;

      runAction->WriteCheckpoint(processed);
      G4cout << "Wrote checkpoint_" << processed << ".csv" << G4endl;
    }
  }

  // Updated to pass SimulationConfig instead of DetectorConstruction
  void RunSweep(G4RunManager* runManager,
                RunAction* runAction,
                SimulationConfig& config) 
  {
    auto* gun = PrimaryGeneratorAction::Instance();
    if (!gun) {
      G4cout << "PrimaryGeneratorAction instance not available." << G4endl;
      return;
    }

    const std::vector<std::string> materials = {
      "G4_Fe",
      "G4_Cu",
      "G4_Al"
    };

    const std::vector<G4double> energies = {
      10.0 * keV,
      15.0 * keV,
      20.0 * keV
    };

    // Pulling events and chunks from config
    const long long totalEvents = config.totalEvents;
    const int chunkSize = config.chunkSize;

    for (const auto& material : materials) {
      // Update config directly instead of calling detector->Set...
      config.sampleMaterial = material;
      runManager->ReinitializeGeometry(true);

      for (const auto& energy : energies) {
        config.beamEnergy = energy;
        gun->SetBeamEnergy(energy); // Ensure gun updates its internal state

        G4cout << "\n========================================\n";
        G4cout << "Running configuration:\n";
        G4cout << "  Material = " << config.sampleMaterial << "\n";
        G4cout << "  Beam energy = " << config.beamEnergy / keV << " keV\n";
        G4cout << "========================================\n";

        RunOneCheckpointedJob(runManager, runAction, totalEvents, chunkSize);
      }
    }
  }
}

int main(int argc, char** argv)
{
  auto* runManager = new G4RunManager();

  // Centralized Configuration Setup
  SimulationConfig config;
  config.sampleMaterial    = "G4_Fe";
  config.beamEnergy        = 20.0 * keV;
  config.incidentAngleDeg  = 45.0;
  config.sourceDistance    = 50.0 * mm;
  config.detectorDistance  = 20.0 * mm;
  config.nominalTakeoffDeg = 45.0;
  config.detectorSpreadDeg = 4.0;
  config.detectorStepDeg   = 2.0;
  config.totalEvents       = 100000;
  config.chunkSize         = 10000;

  // Pass config pointer to detector
  auto* detector = new DetectorConstruction(&config);
  runManager->SetUserInitialization(detector);

  auto* physics = new FTFP_BERT();
  physics->ReplacePhysics(new G4EmLivermorePhysics());
  runManager->SetUserInitialization(physics);

  auto* em = G4EmParameters::Instance();
  em->SetFluo(true);
  em->SetAuger(true);
  em->SetPixe(true);
  em->SetDeexcitationIgnoreCut(true);

  // Pass config pointer to ActionInitialization
  runManager->SetUserInitialization(new ActionInitialization(detector, &config));
  runManager->Initialize();

  auto* runAction = RunAction::Instance();

  // Visualization manager
  auto* visManager = new G4VisExecutive(argc, argv);
  visManager->Initialize();

  auto* uiManager = G4UImanager::GetUIpointer();

  if (argc == 1) {
    // Interactive visualization mode
    auto* ui = new G4UIExecutive(argc, argv);
    uiManager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  }
  else {
    const std::string arg1 = argv[1];

    if (arg1 == "--sweep") {
      // Pass the config down to the sweep routine
      RunSweep(runManager, runAction, config);
    }
    else {
      // Batch macro mode
      uiManager->ApplyCommand("/control/execute " + arg1);
    }
  }

  delete visManager;
  delete runManager;
  return 0;
}