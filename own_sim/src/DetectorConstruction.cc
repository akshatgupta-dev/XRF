#include "DetectorConstruction.hh"
#include "DetectorSD.hh"
#include "SimulationConfig.hh" // <-- Added to include the config structure

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"  // <-- Added for visualization
#include "G4Colour.hh"         // <-- Added for colors

#include <cmath>

// Updated constructor to take the config pointer
DetectorConstruction::DetectorConstruction(SimulationConfig* config)
  : fConfig(config)
{
  UpdateAngleList();
}

// Setters now update fConfig directly
void DetectorConstruction::SetSampleMaterial(const std::string& name)
{
  fConfig->sampleMaterial = name;
}

void DetectorConstruction::SetIncidentAngleDeg(G4double val)
{
  fConfig->incidentAngleDeg = val;
}

void DetectorConstruction::SetSourceDistance(G4double val)
{
  fConfig->sourceDistance = val;
}

void DetectorConstruction::SetDetectorDistance(G4double val)
{
  fConfig->detectorDistance = val;
}

void DetectorConstruction::SetNominalTakeoffDeg(G4double val)
{
  fConfig->nominalTakeoffDeg = val;
  UpdateAngleList();
}

void DetectorConstruction::SetDetectorSpreadDeg(G4double val)
{
  fConfig->detectorSpreadDeg = val;
  UpdateAngleList();
}

void DetectorConstruction::SetDetectorStepDeg(G4double val)
{
  fConfig->detectorStepDeg = val;
  UpdateAngleList();
}

void DetectorConstruction::UpdateAngleList()
{
  fDetectorAnglesDeg.clear();
  
  // Reading from fConfig
  const G4double start = fConfig->nominalTakeoffDeg - fConfig->detectorSpreadDeg;
  const G4double stop  = fConfig->nominalTakeoffDeg + fConfig->detectorSpreadDeg;

  for (G4double a = start; a <= stop + 1e-9; a += fConfig->detectorStepDeg) {
    fDetectorAnglesDeg.push_back(a);
  }
}

G4ThreeVector DetectorConstruction::GetSourcePosition() const
{
  // Reading from fConfig
  const G4double a = fConfig->incidentAngleDeg * deg;
  return G4ThreeVector(
      -fConfig->sourceDistance * std::cos(a),
       0.0,
       fConfig->sourceDistance * std::sin(a)
  );
}

G4ThreeVector DetectorConstruction::GetSourceDirection() const
{
  return (-GetSourcePosition()).unit();
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  auto* nist = G4NistManager::Instance();

  auto* worldMat  = nist->FindOrBuildMaterial("G4_Galactic");
  
  // Reading sample material from fConfig
  auto* sampleMat = nist->FindOrBuildMaterial(fConfig->sampleMaterial);
  auto* detMat    = nist->FindOrBuildMaterial("G4_Si");

  // World
  auto* solidWorld = new G4Box("World", 150*mm, 150*mm, 150*mm);
  auto* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
  auto* physWorld  = new G4PVPlacement(
      nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, true);

  // Set World Visualization Attributes
  auto* worldVis = new G4VisAttributes();
  worldVis->SetVisibility(false); // Hide the world volume to see inside clearly
  logicWorld->SetVisAttributes(worldVis);

  // Sample: flat slab centered at origin, surface normal +z
  auto* solidSample = new G4Box("Sample", 10*mm, 10*mm, 2*mm);
  auto* logicSample = new G4LogicalVolume(solidSample, sampleMat, "Sample");
  new G4PVPlacement(
      nullptr, G4ThreeVector(), logicSample, "Sample", logicWorld, false, 0, true);

  // Set Sample Visualization Attributes
  auto* sampleVis = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));  // red
  sampleVis->SetVisibility(true);
  logicSample->SetVisAttributes(sampleVis);

  // Virtual detector: thin plate that records incoming gammas
  auto* solidDet = new G4Box("VirtualDetector", 2.375*mm, 2.375*mm, 0.25*mm);
  fDetectorLV = new G4LogicalVolume(solidDet, detMat, "VirtualDetectorLV");

  // Set Detector Visualization Attributes
  auto* detVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));     // green
  detVis->SetVisibility(true);
  fDetectorLV->SetVisAttributes(detVis);

  for (G4int i = 0; i < GetNumberOfDetectors(); ++i) {
    const G4double thetaDeg = fDetectorAnglesDeg[i];
    const G4double theta    = thetaDeg * deg;

    // Reading detector distance from fConfig
    const G4ThreeVector pos(
        fConfig->detectorDistance * std::cos(theta),
        0.0,
        fConfig->detectorDistance * std::sin(theta));

    auto* rot = new G4RotationMatrix();
    rot->rotateY((90.0*deg + theta)); // face the origin

    new G4PVPlacement(
        rot,
        pos,
        fDetectorLV,
        "VirtualDetectorPV",
        logicWorld,
        false,
        i,     // copy number = detector ID
        true);
  }

  return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
  auto* sd = new DetectorSD("DetectorSD");
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);
  SetSensitiveDetector(fDetectorLV, sd);
}