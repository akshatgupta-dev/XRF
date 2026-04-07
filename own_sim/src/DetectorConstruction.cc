#include "DetectorConstruction.hh"
#include "DetectorSD.hh"
#include "SimulationConfig.hh" 

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"  
#include "G4Colour.hh"         

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

  // =========================================================================
  // 2D PIXEL GRID DETECTOR ARRAY
  // =========================================================================

  const G4double theta0 = fConfig->nominalTakeoffDeg * deg;
  const G4double D      = fConfig->detectorDistance;
  const G4double maxOff = 4.0 * deg;

  const G4double capRadius = D * std::tan(maxOff);
  const G4double pixelSize = 2 * mm;   // choose resolution
  const G4double halfT     = 0.05 * mm;

  auto* solidPix = new G4Box("Pixel", pixelSize/2.0, pixelSize/2.0, halfT);
  fDetectorLV = new G4LogicalVolume(solidPix, detMat, "PixelLV");

  auto* detVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  detVis->SetVisibility(true);
  fDetectorLV->SetVisAttributes(detVis);

  G4ThreeVector detCenter(
      D * std::cos(theta0),
      0.0,
      D * std::sin(theta0)
  );

  G4RotationMatrix rot0;
  rot0.rotateY(-(90.0 * deg + theta0));

G4RotationMatrix placeRot = rot0.inverse(); 

  const int n = static_cast<int>(std::ceil((2.0 * capRadius) / pixelSize));

  int copyNo = 0;
  for (int ix = 0; ix < n; ++ix) {
      G4double x = -capRadius + (ix + 0.5) * pixelSize;

      for (int iy = 0; iy < n; ++iy) {
          G4double y = -capRadius + (iy + 0.5) * pixelSize;

          if (x*x + y*y > capRadius*capRadius) continue;

          G4ThreeVector local(x, y, 0.0);
          G4ThreeVector global = detCenter + rot0 * local; // Uses active rotation for position

          new G4PVPlacement(
              new G4RotationMatrix(placeRot), // <-- Use the INVERSE rotation here
              global,
              fDetectorLV,
              "PixelPV",
              logicWorld,
              false,
              copyNo++,
              true
          );
      }
  }

  return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
  auto* sd = new DetectorSD("DetectorSD");
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);
  SetSensitiveDetector(fDetectorLV, sd);
}