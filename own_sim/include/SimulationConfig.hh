#ifndef SimulationConfig_h
#define SimulationConfig_h 1
#include <string>
#include <vector>
#include "globals.hh"
#include "G4SystemOfUnits.hh"

#include <string>
struct NamedComboConfig {
  std::string name;
  std::vector<G4int> detIds;
};

struct ShieldLayerConfig {
    std::string material;
    G4double thickness;
    G4double gapBefore;   // gap from previous layer to this layer

    ShieldLayerConfig(const std::string& mat = "G4_Pb",
                      G4double t = 1.0 * mm,
                      G4double gap = 0.0 * mm)
      : material(mat), thickness(t), gapBefore(gap) {}
};

struct ShieldConfig {
    G4bool enabled;
    G4double detectorGap; // detector -> first layer
    std::vector<ShieldLayerConfig> layers;

    ShieldConfig()
      : enabled(true),
        detectorGap(10.0 * mm),
        layers{
            ShieldLayerConfig("G4_Al", 0.25 * mm, 0.0 * mm),
            ShieldLayerConfig("G4_Cu", 0.25 * mm, 0.0 * mm),
            ShieldLayerConfig("G4_Sn", 0.50 * mm, 0.0 * mm),
            ShieldLayerConfig("G4_Pb", 2.00 * mm, 0.0 * mm)
        } {}
};

struct SimulationConfig
{
  std::string sampleMaterial = "G4_Fe";

  G4double beamEnergy        = 20.0 * keV;

  G4double incidentAngleDeg  = 45.0;
  G4double sourceDistance    = 50.0 * mm;
  G4double detectorDistance  = 20.0 * mm;

  G4double nominalTakeoffDeg = 45.0;
  G4double detectorSpreadDeg = 4.0;
  G4double detectorStepDeg   = 2.0;

  long long totalEvents      = 100000;
  int chunkSize              = 10000;
std::vector<G4int> comboSizes = {1, 3, 5, 9};
std::vector<NamedComboConfig> customCombos;

  ShieldConfig shield;
  
  std::string BuildRunLabel() const;
  std::string BuildCheckpointFilename(long long processedEvents) const;
};

#endif