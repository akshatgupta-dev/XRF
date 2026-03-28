#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"

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

  void RunSweep(G4RunManager* runManager,
                DetectorConstruction* detector,
                RunAction* runAction)
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

    const long long totalEvents = 100000;
    const int chunkSize = 10000;

    for (const auto& material : materials) {
      detector->SetSampleMaterial(material);
      runManager->ReinitializeGeometry(true);

      for (const auto& energy : energies) {
        gun->SetBeamEnergy(energy);

        G4cout << "\n========================================\n";
        G4cout << "Running configuration:\n";
        G4cout << "  Material = " << material << "\n";
        G4cout << "  Beam energy = " << energy / keV << " keV\n";
        G4cout << "========================================\n";

        RunOneCheckpointedJob(runManager, runAction, totalEvents, chunkSize);
      }
    }
  }
}

int main(int argc, char** argv)
{
  auto* runManager = new G4RunManager();

  auto* detector = new DetectorConstruction();

  // Current validated geometry
  detector->SetSampleMaterial("G4_Fe");
  detector->SetIncidentAngleDeg(45.0);
  detector->SetSourceDistance(50.0);      // mm
  detector->SetDetectorDistance(150.0);    // mm
  detector->SetNominalTakeoffDeg(45.0);
  detector->SetDetectorSpreadDeg(4.0);
  detector->SetDetectorStepDeg(2.0);

  runManager->SetUserInitialization(detector);

  auto* physics = new FTFP_BERT();
  physics->ReplacePhysics(new G4EmLivermorePhysics());
  runManager->SetUserInitialization(physics);

  auto* em = G4EmParameters::Instance();
  em->SetFluo(true);
  em->SetAuger(true);
  em->SetPixe(true);
  em->SetDeexcitationIgnoreCut(true);

  runManager->SetUserInitialization(new ActionInitialization(detector));
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
      RunSweep(runManager, detector, runAction);
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