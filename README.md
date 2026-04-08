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