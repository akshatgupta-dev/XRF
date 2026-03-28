#include "DetectorConstruction.hh"
#include "DetectorSD.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

#include <cmath>

DetectorConstruction::DetectorConstruction()
{
  UpdateAngleList();
}

void DetectorConstruction::SetSampleMaterial(const std::string& name)
{
  fSampleMaterial = name;
}

void DetectorConstruction::SetIncidentAngleDeg(G4double val)
{
  fIncidentAngleDeg = val;
}

void DetectorConstruction::SetSourceDistance(G4double val)
{
  fSourceDistance = val;
}

void DetectorConstruction::SetDetectorDistance(G4double val)
{
  fDetectorDistance = val;
}

void DetectorConstruction::SetNominalTakeoffDeg(G4double val)
{
  fNominalTakeoffDeg = val;
  UpdateAngleList();
}

void DetectorConstruction::SetDetectorSpreadDeg(G4double val)
{
  fDetectorSpreadDeg = val;
  UpdateAngleList();
}

void DetectorConstruction::SetDetectorStepDeg(G4double val)
{
  fDetectorStepDeg = val;
  UpdateAngleList();
}

void DetectorConstruction::UpdateAngleList()
{
  fDetectorAnglesDeg.clear();
  const G4double start = fNominalTakeoffDeg - fDetectorSpreadDeg;
  const G4double stop  = fNominalTakeoffDeg + fDetectorSpreadDeg;

  for (G4double a = start; a <= stop + 1e-9; a += fDetectorStepDeg) {
    fDetectorAnglesDeg.push_back(a);
  }
}

G4ThreeVector DetectorConstruction::GetSourcePosition() const
{
  const G4double a = fIncidentAngleDeg * deg;
  return G4ThreeVector(
      -fSourceDistance * std::cos(a),
       0.0,
       fSourceDistance * std::sin(a)
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
  auto* sampleMat = nist->FindOrBuildMaterial(fSampleMaterial);
  auto* detMat    = nist->FindOrBuildMaterial("G4_Si");

  // World
  auto* solidWorld = new G4Box("World", 150*mm, 150*mm, 150*mm);
  auto* logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
  auto* physWorld  = new G4PVPlacement(
      nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, true);

  // Sample: flat slab centered at origin, surface normal +z
  auto* solidSample = new G4Box("Sample", 10*mm, 10*mm, 2*mm);
  auto* logicSample = new G4LogicalVolume(solidSample, sampleMat, "Sample");
  new G4PVPlacement(
      nullptr, G4ThreeVector(), logicSample, "Sample", logicWorld, false, 0, true);

  // Virtual detector: thin plate that records incoming gammas
auto* solidDet = new G4Box("VirtualDetector", 2.375*mm, 2.375*mm, 0.25*mm);
  fDetectorLV = new G4LogicalVolume(solidDet, detMat, "VirtualDetectorLV");

  for (G4int i = 0; i < GetNumberOfDetectors(); ++i) {
    const G4double thetaDeg = fDetectorAnglesDeg[i];
    const G4double theta    = thetaDeg * deg;

    const G4ThreeVector pos(
        fDetectorDistance * std::cos(theta),
        0.0,
        fDetectorDistance * std::sin(theta));

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