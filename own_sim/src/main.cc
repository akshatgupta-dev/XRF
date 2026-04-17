#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"

#include "SimulationConfig.hh" // <-- Added Config header

#include "XRFMessenger.hh"
#include "FTFP_BERT.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmParameters.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4ios.hh"
#include "G4StateManager.hh"

#include <algorithm>
#include <string>
#include <vector>
#include "G4Timer.hh"
#include <chrono>

namespace
{
  void RunOneCheckpointedJob(G4RunManager* runManager,
                            DetectorConstruction* detector,
                            RunAction* runAction,
                            const SimulationConfig& config)
  {
    detector->RefreshDetectorLayout();
    runAction->SyncWithDetectorLayout();
    runAction->ResetAll();

    long long processed = 0;

    while (processed < config.totalEvents) {
      const int thisChunk =
          static_cast<int>(std::min<long long>(config.chunkSize,
                                              config.totalEvents - processed));

      G4cout << "Starting chunk with " << thisChunk
            << " events. Processed so far = " << processed << G4endl;

      // total wall-clock time for this chunk
      auto chunkWallStart = std::chrono::steady_clock::now();

      // pure simulation time for this chunk
      G4Timer simTimer;
      simTimer.Start();

      runManager->BeamOn(thisChunk);

      simTimer.Stop();

      processed += thisChunk;

      auto writeStart = std::chrono::steady_clock::now();
      runAction->WriteCheckpoint(processed);
      auto writeEnd = std::chrono::steady_clock::now();

      auto chunkWallEnd = std::chrono::steady_clock::now();

      const double simReal =
          simTimer.GetRealElapsed();

      const double writeWall =
          std::chrono::duration<double>(writeEnd - writeStart).count();

      const double chunkWall =
          std::chrono::duration<double>(chunkWallEnd - chunkWallStart).count();

      G4cout << "\n=== Chunk timing summary ===" << G4endl;
      G4cout << "Events simulated in chunk : " << thisChunk << G4endl;
      G4cout << "Pure simulation time      : " << simReal << " s" << G4endl;
      G4cout << "Checkpoint write time     : " << writeWall << " s" << G4endl;
      G4cout << "Total chunk wall time     : " << chunkWall << " s" << G4endl;
      if (thisChunk > 0) {
        G4cout << "Sim time / event          : " << simReal / thisChunk << " s" << G4endl;
        G4cout << "Chunk time / event        : " << chunkWall / thisChunk << " s" << G4endl;
      }
      G4cout << "============================\n" << G4endl;

      if (processed >= config.totalEvents) {
        runAction->FinalizeRun(processed);
      }
    }
  }

  // Updated to pass SimulationConfig instead of DetectorConstruction
  void RunSweep(G4RunManager* runManager,
              DetectorConstruction* detector,
              RunAction* runAction,
              SimulationConfig& config) 
  {
    auto* gun = PrimaryGeneratorAction::Instance();
    if (!gun) {
      G4cout << "PrimaryGeneratorAction instance not available." << G4endl;
      return;
    }

    const std::vector<std::string> materials = {
      "G4_Fe"
      // "G4_Cu",
      // "G4_Al"
    };

    const std::vector<G4double> energies = {
      // 10.0 * keV,
      // 15.0 * keV,
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

        RunOneCheckpointedJob(runManager, detector, runAction, config);
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
  config.incidentAngleDeg  = 50.0;
  config.sourceDistance    = 60.0 * mm;
  config.detectorDistance  = 30.0 * mm;
  config.nominalTakeoffDeg = 47.0;
  config.detectorSpreadDeg = 4.0;
  config.detectorStepDeg   = 2.0;
  config.totalEvents       = 10000;
  config.chunkSize         = 1000;

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

  auto ensureInitialized = [&]() -> RunAction* {
    if (G4StateManager::GetStateManager()->GetCurrentState() == G4State_PreInit) {
      runManager->Initialize();
    }
    return RunAction::Instance();
  };

  auto* xrfMessenger = new XRFMessenger(
    &config,
    detector,
    [&]() {
      auto* runAction = ensureInitialized();
      if (!runAction) {
        G4cout << "RunAction instance not available." << G4endl;
        return;
      }

      runManager->ReinitializeGeometry(true);
      auto* gun = PrimaryGeneratorAction::Instance();
      if (gun) {
        gun->SetBeamEnergy(config.beamEnergy);
      }
      RunOneCheckpointedJob(runManager, detector, runAction, config);
    });
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
      auto* runAction = ensureInitialized();
      if (!runAction) {
        G4cout << "RunAction instance not available." << G4endl;
      }
      else {
      RunSweep(runManager, detector, runAction, config);
      }
    }
    else {
      // Batch macro mode
      uiManager->ApplyCommand("/control/execute " + arg1);
    }
  }

  delete visManager;
  delete xrfMessenger;
  delete runManager;
  return 0;
}