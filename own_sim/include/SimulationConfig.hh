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
  std::string BuildRunLabel() const;
  std::string BuildCheckpointFilename(long long processedEvents) const;
};

#endif