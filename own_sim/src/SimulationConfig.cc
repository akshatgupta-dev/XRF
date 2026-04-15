#include "SimulationConfig.hh"

#include <iomanip>
#include <sstream>
#include <string>

namespace
{
  // Helper function to replace problem characters in filenames
  std::string Sanitize(std::string s)
  {
    for (char& c : s) {
      if (c == '.') c = 'p';
      else if (c == ' ') c = '_';
      else if (c == '/') c = '_';
    }
    return s;
  }

  // Helper function to format doubles to 1 decimal place
  std::string Format1(double v)
  {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << v;
    return oss.str();
  }

  std::string Format2(double v)
  {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << v;
    return oss.str();
  }
}

std::string SimulationConfig::BuildSampleMaterialTag() const
{
  if (!sampleMaterialIsCustom || sampleMaterialComponents.empty()) {
    return Sanitize(sampleMaterial);
  }

  std::ostringstream oss;
  oss << Sanitize(sampleMaterial)
      << "_mix"
      << "_rho" << Sanitize(Format2(sampleMaterialDensity / (g/cm3))) << "gcm3";

  for (const auto& component : sampleMaterialComponents) {
    oss << "_" << Sanitize(component.element)
        << Sanitize(Format2(component.fraction * 100.0)) << "pct";
  }

  return oss.str();
}

std::string SimulationConfig::BuildRunLabel() const
{
  std::ostringstream oss;
  oss << BuildSampleMaterialTag()
      << "_E"    << Sanitize(Format1(beamEnergy / keV)) << "keV"
      << "_In"   << Sanitize(Format1(incidentAngleDeg)) << "deg"
      << "_Src"  << Sanitize(Format1(sourceDistance / mm)) << "mm"
      << "_Take" << Sanitize(Format1(nominalTakeoffDeg)) << "deg"
      << "_Det"  << Sanitize(Format1(detectorDistance / mm)) << "mm"
      << "_Spr"  << Sanitize(Format1(detectorSpreadDeg)) << "deg"
      << "_Step" << Sanitize(Format1(detectorStepDeg)) << "deg";

  if (!shield.enabled || shield.layers.empty()) {
    oss << "_ShieldOff";
  }
  else {
    oss << "_ShGap" << Sanitize(Format1(shield.detectorGap / mm)) << "mm";
    for (const auto& layer : shield.layers) {
        oss << "_" << Sanitize(layer.material)
            << "x" << Sanitize(Format1(layer.thickness / mm)) << "mm";
        if (layer.gapBefore > 0.0) {
            oss << "g" << Sanitize(Format1(layer.gapBefore / mm)) << "mm";
        }
    }
  }
  return oss.str();
}

std::string SimulationConfig::BuildCheckpointFilename(long long processedEvents) const
{
  std::ostringstream oss;
  oss << "output/" << BuildSampleMaterialTag() << "/"
      << BuildRunLabel() << "_N" << processedEvents << ".csv";
  return oss.str();
}