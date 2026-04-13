# XRF Simulation (Geant4)

This project contains a Geant4-based simulation for X-ray fluorescence (XRF). It is designed to simulate interactions between incident X-rays and materials, and to generate output data for further analysis.

## Features

- Simulates X-ray interactions with different materials
- Configurable beam energy and geometry
- Outputs results in `.csv` format
- Supports multiple runs and detector configurations

## Requirements

- Geant4
- CMake
- C++ compiler (GCC/Clang)

## Build Instructions

```bash
git clone https://github.com/akshatgupta-dev/XRF.git
cd XRF
mkdir build && cd build
cmake ..
make
```

## Run
```bash
./your_executable
```


## Output
- Simulation results are stored in the output/ directory
- Files are saved in CSV format for easy analysis


## Notes
- Parameters such as energy, angles, and number of events can be modified in the source files
- Make sure Geant4 environment variables are properly set before building


example


```bash
/xrf/clearCustomCombos
/xrf/addCombo center_only 6
/xrf/addCombo center_3 5 6 7
/xrf/addCombo center_5 4 5 6 7 8
/xrf/addCombo upper_row 9 10 11 12
/xrf/addCombo lower_row 0 1 2 3
/xrf/addCombo left_side 1 4 5 9
/xrf/addCombo right_side 3 7 8 11
```


layer order is inner to outward



## **1. Material & Beam Settings**

| Command | Parameter | Type | Example |
|---------|-----------|------|---------|
| `/xrf/setMaterial` | material name | string | `/xrf/setMaterial G4_Fe` |
| `/xrf/setBeamEnergy` | energy + unit | double + unit | `/xrf/setBeamEnergy 20 keV` |

* **Available Materials:** `G4_Fe`, `G4_Cu`, `G4_Pb`, `G4_Al`, `G4_Au`, `G4_Ag`, `G4_Sn`, `G4_Ni`, `G4_Cr`, etc.
* **Energy Units:** `eV`, `keV`, `MeV`

---

## **2. Simulation Parameters**

| Command | Parameter | Type | Example |
|---------|-----------|------|---------|
| `/xrf/setTotalEvents` | number of events | integer | `/xrf/setTotalEvents 50000` |
| `/xrf/setChunkSize` | events per chunk | integer | `/xrf/setChunkSize 1000` |

---

## **3. Geometry: Detector Positioning**

| Command | Parameter | Type | Example |
|---------|-----------|------|---------|
| `/xrf/setDetectorDistance` | distance + unit | double + unit | `/xrf/setDetectorDistance 50 mm` |
| `/xrf/setSourceDistance` | distance + unit | double + unit | `/xrf/setSourceDistance 100 mm` |
| `/xrf/setIncidentAngle` | angle + unit | double + unit | `/xrf/setIncidentAngle 45 deg` |

* **Length Units:** `mm`, `cm`, `um`, `nm`
* **Angle Units:** `deg`, `rad`

---

## **4. Geometry: Detector Angles**

| Command | Parameter | Type | Example |
|---------|-----------|------|---------|
| `/xrf/setNominalTakeoff` | angle + unit | double + unit | `/xrf/setNominalTakeoff 35 deg` |
| `/xrf/setSpread` | angle + unit | double + unit | `/xrf/setSpread 10 deg` |
| `/xrf/setStep` | angle + unit | double + unit | `/xrf/setStep 1 deg` |

---

## **5. Shield Configuration**

| Command | Parameters | Type | Example |
|---------|-----------|------|---------|
| `/xrf/setShieldEnabled` | true/false | boolean | `/xrf/setShieldEnabled true` |
| `/xrf/setShieldDetectorGap` | gap + unit | double + unit | `/xrf/setShieldDetectorGap 2 mm` |
| `/xrf/addShieldLayer` | material thickness unit [gapBefore] [gapUnit] | string + values | `/xrf/addShieldLayer G4_Pb 2 mm 1 mm` |
| `/xrf/clearShieldLayers` | (none) | (none) | `/xrf/clearShieldLayers` |

* **Note on `addShieldLayer`:** The `gapBefore` parameter specifies the air gap *before* that specific layer. If omitted, it defaults to 0.

---

## **6. Detector Combinations**

| Command | Parameters | Type | Example |
|---------|-----------|------|---------|
| `/xrf/comboSizes` | space-separated integers | string | `/xrf/comboSizes 1 3 5 9` |
| `/xrf/addCombo` | name id1 id2 id3... | string + integers | `/xrf/addCombo combo_3x3 0 1 2 3 4 5 6 7 8` |
| `/xrf/clearCustomCombos` | (none) | (none) | `/xrf/clearCustomCombos` |

---

## **7. Execution**

| Command | Parameters | Type | Example |
|---------|-----------|------|---------|
| `/xrf/run` | (none) | (none) | `/xrf/run` |

---

## **Complete Example Macro**

```geant4
# ========== MATERIAL & BEAM ==========
/xrf/setMaterial G4_Fe
/xrf/setBeamEnergy 20 keV

# ========== SIMULATION ==========
/xrf/setTotalEvents 50000
/xrf/setChunkSize 1000

# ========== DETECTOR POSITIONING ==========
/xrf/setSourceDistance 100 mm
/xrf/setDetectorDistance 50 mm
/xrf/setIncidentAngle 45 deg

# ========== DETECTOR ANGLES ==========
/xrf/setNominalTakeoff 35 deg
/xrf/setSpread 10 deg
/xrf/setStep 1 deg

# ========== SHIELD SETUP ==========
/xrf/clearShieldLayers
/xrf/setShieldEnabled true
/xrf/setShieldDetectorGap 2 mm
/xrf/addShieldLayer G4_Pb 2 mm 1 mm
/xrf/addShieldLayer G4_Sn 1 mm 0.5 mm

# ========== DETECTOR COMBOS ==========
/xrf/comboSizes 1 3 5
/xrf/clearCustomCombos
/xrf/addCombo custom_pair 0 2
/xrf/addCombo custom_trio 1 3 5

# ========== EXECUTE ==========
/xrf/run




## **for the ansto library**

/physics_lists/em/ReplacePhysics G4EmStandardPhysics_option4
/run/initialize

/process/em/fluo true
/process/em/auger true
/process/em/pixe true
/process/em/fluoDirectory ansto
/process/em/pixeXSmodel ECPSSR_ANSTO
/process/em/deexcitationIgnoreCut true

/run/setCut 0.001 mm

# Example source: 50 keV photon beam hitting your sample
/gps/particle gamma
/gps/energy 50 keV
/run/beamOn 100000
