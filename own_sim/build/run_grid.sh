#!/usr/bin/env bash
set -e

SIM=./sim
MACRO=tmp_run.mac

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
          done
        done
      done
    done
  done
done