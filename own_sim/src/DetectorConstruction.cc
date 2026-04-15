#include "DetectorConstruction.hh"
#include "DetectorSD.hh"
#include "PMSensitiveDetector.hh"
#include "SimulationConfig.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include <algorithm>
#include <cmath>
#include <vector>

DetectorConstruction::DetectorConstruction(SimulationConfig* config)
  : fConfig(config)
{
  RebuildDetectorElements();
}

// Setters now update fConfig directly without updating the old angle list
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
  RebuildDetectorElements();
}

void DetectorConstruction::SetNominalTakeoffDeg(G4double val)
{
  fConfig->nominalTakeoffDeg = val;
  RebuildDetectorElements();
}

void DetectorConstruction::SetDetectorSpreadDeg(G4double val)
{
  fConfig->detectorSpreadDeg = val;
  RebuildDetectorElements();
}

void DetectorConstruction::SetDetectorStepDeg(G4double val)
{
  fConfig->detectorStepDeg = val;
  RebuildDetectorElements();
}
G4double DetectorConstruction::GetDetectorCapRadius() const
{
  const G4double D = fConfig->detectorDistance;
  const G4double spreadDeg = std::max(0.0, fConfig->detectorSpreadDeg)*deg;
  G4double rest_angle=(180-(90+spreadDeg))*deg ;
  
  G4double radius=std::sin(spreadDeg)*D/std::sin(rest_angle);

  return radius;

}


G4double DetectorConstruction::GetDetectorPixelSize() const
{
  const G4double D = fConfig->detectorDistance;

  // Use detectorStepDeg as angular spacing between element centers.
  const G4double stepDeg = std::max(0.1, fConfig->detectorStepDeg);

  // Linear size/pitch corresponding to that angular step
  return 2.0 * D * std::tan(0.5 * stepDeg * deg);
}

void DetectorConstruction::RefreshDetectorLayout()
{
  RebuildDetectorElements();
}

void DetectorConstruction::SetShieldEnabled(G4bool val)
{
  fConfig->shield.enabled = val;
}

void DetectorConstruction::SetShieldDetectorGap(G4double val)
{
  fConfig->shield.detectorGap = std::max(0.0, val);
}

void DetectorConstruction::ClearShieldLayers()
{
  fConfig->shield.layers.clear();
}

void DetectorConstruction::AddShieldLayer(const std::string& material,
                                          G4double thickness,
                                          G4double gapBefore)
{
  if (material.empty() || thickness <= 0.0) {
    return;
  }

  ShieldLayerConfig layer(material, thickness, std::max(0.0, gapBefore));
  fConfig->shield.layers.push_back(layer);
}

void DetectorConstruction::RebuildDetectorElements()
{
    fDetectorElements.clear();

    const G4double theta0 = fConfig->nominalTakeoffDeg * deg;
    const G4double D = fConfig->detectorDistance;
    const G4double capRadius = GetDetectorCapRadius();
    const G4double pixelSize = GetDetectorPixelSize();

    G4ThreeVector detCenter(
        D * std::cos(theta0),
        0.0,
        D * std::sin(theta0)
    );

    G4RotationMatrix rot0;
    rot0.rotateY(-(90.0 * deg + theta0));

    const int n = std::max(0, static_cast<int>(std::ceil(capRadius / pixelSize)));

    for (int ix = -n; ix <= n; ++ix) {
        const G4double x = ix * pixelSize;

        for (int iy = -n; iy <= n; ++iy) {
            const G4double y = iy * pixelSize;

            if (x*x + y*y > capRadius*capRadius) {
                continue;
            }

            G4ThreeVector local(x, y, 0.0);
            G4ThreeVector global = detCenter + rot0 * local;

            DetectorElement elem;
            elem.id = static_cast<G4int>(fDetectorElements.size());
            elem.center = global;
            elem.thetaDeg = ComputeThetaDeg(global);
            elem.phiDeg = ComputePhiDeg(global);
            elem.width = pixelSize;
            elem.height = pixelSize;
            elem.enabled = true;
            fDetectorElements.push_back(elem);
        }
    }

    // Guarantee at least one detector element
    if (fDetectorElements.empty()) {
        DetectorElement elem;
        elem.id = 0;
        elem.center = detCenter;
        elem.thetaDeg = ComputeThetaDeg(detCenter);
        elem.phiDeg = ComputePhiDeg(detCenter);
        elem.width = pixelSize;
        elem.height = pixelSize;
        elem.enabled = true;
        fDetectorElements.push_back(elem);
    }

    G4cout << "DEBUG RebuildDetectorElements: built "
           << fDetectorElements.size() << " detector elements." << G4endl;
}

G4double DetectorConstruction::ComputeThetaDeg(const G4ThreeVector& p) const {
    return std::atan2(p.z(), p.x()) / deg;
}

G4double DetectorConstruction::ComputePhiDeg(const G4ThreeVector& p) const {
    return std::atan2(p.y(), p.x()) / deg;
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
  RebuildDetectorElements();
  
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

const G4double pixelSize = GetDetectorPixelSize();
  const G4double halfT     = 0.05 * mm;

  auto* solidPix = new G4Box("Pixel", pixelSize/2.0, pixelSize/2.0, halfT);
  fDetectorLV = new G4LogicalVolume(solidPix, detMat, "PixelLV");

  auto* detVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  detVis->SetVisibility(true);
  fDetectorLV->SetVisAttributes(detVis);

  const G4double theta0 = fConfig->nominalTakeoffDeg * deg;
  G4RotationMatrix rot0;
  rot0.rotateY(-(90.0 * deg + theta0));
  G4RotationMatrix placeRot = rot0.inverse(); 

  for (const auto& elem : fDetectorElements) {
      new G4PVPlacement(
          new G4RotationMatrix(placeRot),
          elem.center,
          fDetectorLV,
          "PixelPV",
          logicWorld,
          false,
          elem.id,
          true
      );
  }


//  inner Al (0.25 mm) -> Cu (0.25 mm) -> Sn (0.5 mm) -> Pb/W (2.0 mm) outward



  
    const auto& shield = fConfig->shield;

    if (shield.enabled && !shield.layers.empty()) {

        const G4double detHalfT  = 0.05 * mm;
        const G4double detectorGap = std::max(0.0, shield.detectorGap);
        const G4double capRadius = GetDetectorCapRadius();

        const G4double innerHalfX = capRadius + 0.5 * pixelSize + detectorGap;
        const G4double innerHalfY = capRadius + 0.5 * pixelSize + detectorGap;

        const G4double D = fConfig->detectorDistance;
        G4ThreeVector detCenter(
            D * std::cos(theta0),
            0.0,
            D * std::sin(theta0)
        );

        G4RotationMatrix shieldRot = placeRot;

        auto colourFor = [](const std::string& mat) -> G4Colour {
            if (mat == "G4_Al") return G4Colour(0.80, 0.80, 0.85);
            if (mat == "G4_Cu") return G4Colour(0.80, 0.45, 0.20);
            if (mat == "G4_Sn") return G4Colour(0.60, 0.60, 0.70);
            if (mat == "G4_Pb") return G4Colour(0.30, 0.30, 0.35);
            if (mat == "G4_W")  return G4Colour(0.20, 0.20, 0.25);
            return G4Colour(0.50, 0.50, 0.50);
        };

        G4double currentHalfX = innerHalfX;
        G4double currentHalfY = innerHalfY;
        G4double currentBack  = detectorGap;

        for (size_t i = 0; i < shield.layers.size(); ++i) {
          const auto& layer = shield.layers[i];
          G4cout << "Building shield layer " << i
               << " material=" << layer.material
               << " thickness=" << layer.thickness / mm << " mm"
               << " gapBefore=" << layer.gapBefore / mm << " mm"
               << G4endl;

          auto* mat = nist->FindOrBuildMaterial(layer.material);
          if (!mat) {
            G4cout << "WARNING: shield material not found: "
                 << layer.material << G4endl;
            continue;
          }

          const G4double gap = std::max(0.0, layer.gapBefore);
          const G4double t   = layer.thickness;

          currentHalfX += gap;
          currentHalfY += gap;
          currentBack  += gap;

          const G4double outerHalfX = currentHalfX + t;
          const G4double outerHalfY = currentHalfY + t;
          const G4double outerBack  = currentBack  + t;

          const G4String base = "Shield_" + std::to_string(i);

          auto* outerSolid = new G4Box(base + "_Outer_Solid",
                         outerHalfX,
                         outerHalfY,
                         (currentBack + t) / 2.0);
          auto* innerSolid = new G4Box(base + "_Inner_Solid",
                         currentHalfX,
                         currentHalfY,
                         currentBack / 2.0);

          auto* shieldSolid = new G4SubtractionSolid(
            base + "_Shell_Solid",
            outerSolid,
            innerSolid,
            nullptr,
            G4ThreeVector(0.0, 0.0, t / 2.0)
          );

          auto* logic = new G4LogicalVolume(shieldSolid, mat, base + "_LV");
          auto* vis   = new G4VisAttributes(colourFor(layer.material));
          vis->SetVisibility(true);
          logic->SetVisAttributes(vis);

          G4ThreeVector localPos(
            0.0,
            0.0,
            -(detHalfT + (currentBack + t) / 2.0)
          );
          G4ThreeVector globalPos = detCenter + rot0 * localPos;

          new G4PVPlacement(new G4RotationMatrix(shieldRot),
                    globalPos,
                    logic,
                    base + "_PV",
                    logicWorld,
                    false,
                    static_cast<G4int>(1000 + i),
                    true);

          currentHalfX = outerHalfX;
          currentHalfY = outerHalfY;
          currentBack  = outerBack;
        }

      }

  return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
  auto* sd = new PMSensitiveDetector("PMSensitiveDetector");
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);
  SetSensitiveDetector(fDetectorLV, sd);
}