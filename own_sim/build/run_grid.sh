#!/usr/bin/env bash
set -e

SIM=./sim
MACRO=tmp_run.mac

# Ensure output directory exists
mkdir -p output

manifest="output/manifest.csv"

# Initialize manifest with headers if it doesn't exist
if [ ! -f "$manifest" ]; then
  echo "filename,material,energy_keV,incident_deg,source_mm,detector_mm,takeoff_deg,spread_deg,step_deg,processed_events" > "$manifest"
fi

materials=("G4_Fe" "G4_Cu")
energies=(15 20)
incident_angles=(45 50)
source_distances=(50 60)
detector_distances=(20 30)
takeoffs=(45 47)

total_events=10000
chunk_size=1000
spread=4
step=2

for mat in "${materials[@]}"; do
  for E in "${energies[@]}"; do
    for inc in "${incident_angles[@]}"; do
      for src in "${source_distances[@]}"; do
        for det in "${detector_distances[@]}"; do
          for take in "${takeoffs[@]}"; do
            
            # Generate the macro file
            cat > "$MACRO" <<EOF
/control/verbose 1
/run/verbose 1
/xrf/setMaterial $mat
/xrf/setBeamEnergy $E keV
/xrf/setIncidentAngle $inc deg
/xrf/setSourceDistance $src mm
/xrf/setDetectorDistance $det mm
/xrf/setNominalTakeoff $take deg
/xrf/setSpread $spread deg
/xrf/setStep $step deg
/xrf/setTotalEvents $total_events
/xrf/setChunkSize $chunk_size
/xrf/run
EOF
            
            echo "Running: $mat E=${E}keV In=${inc} Take=${take} Src=${src} Det=${det}"
            $SIM "$MACRO"
            
            # Append checkpoint records to the manifest
            for n in $(seq $chunk_size $chunk_size $total_events); do
              # Replaced .0 with p0 to match C++ output
              fname="${mat}_E${E}p0keV_In${inc}p0deg_Src${src}p0mm_Take${take}p0deg_Det${det}p0mm_Spr${spread}p0deg_Step${step}p0deg_N${n}.csv"
              
              # Added ${mat}/ to the path to match the subdirectory
              echo "output/${mat}/${fname},${mat},${E},${inc},${src},${det},${take},${spread},${step},${n}" >> "$manifest"
            done
            
          done
        done
      done
    done
  done
done