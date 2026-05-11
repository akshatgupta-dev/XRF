import spekpy as sp
import pandas as pd 
import matplotlib.pyplot as plt 
import subprocess
import os 
import uproot
import h5py
import numpy as np

GUN_TO_SAMPLE_DISTANCE=10 #mm
CURRENT=0.001 #current unit is mA
TUBE_VOLTAGE=50
ANODE_ANGLE_DEGREE=12
ANODE_TARGET_MATERIAL="W"

SAMPLE_HEIGHT=5 #mm
SAMPLE_WIDTH=5 #mm
Pileup_Time_Window=100 # in ns

NUMBER_OF_EVENT=100000
THREAD=10
OUTPUT_FOLDER_NAME="FE_100"
OUTPUT_FILE_NAME="SECOND_test.root"

focalspot=0.5 #mm


os.makedirs(f"../output_file/{OUTPUT_FOLDER_NAME}",exist_ok=True)
final_output_folder_path=os.path.join("../output_file",OUTPUT_FOLDER_NAME)

root_folder_path=os.path.join(final_output_folder_path,OUTPUT_FILE_NAME)

s=sp.Spek(kvp=TUBE_VOLTAGE,th=ANODE_ANGLE_DEGREE,targ=ANODE_TARGET_MATERIAL)
s.filter("Be", 0.125) #Tube window
fluence_at_1m=s.get_flu()#Photons / cm² / mAs (at a 1-meter distance)

fluence_at_detector_distance=fluence_at_1m*(100/(GUN_TO_SAMPLE_DISTANCE/10))**2

area_of_sample=SAMPLE_HEIGHT/10*SAMPLE_WIDTH/10

phton_per_second=fluence_at_detector_distance*CURRENT*area_of_sample

print(phton_per_second)
mean_gap_s =1/phton_per_second
mean_gap_s*=1e9

energy,fluence_weight=s.get_spectrum()
plt.plot(energy,fluence_weight)
plt.show()

energy/=1000.0

os.makedirs("../output_file",exist_ok=True)


MACRO=f"""

/xrf/sampleMaterial G4_Fe
/xrf/sampleMaterialDensity 7.874 g/cm3
/xrf/incidentAngle 45 deg
/xrf/sourceDistance {GUN_TO_SAMPLE_DISTANCE} mm
/xrf/detectorDistance 5 mm
/xrf/takeoffAngle 45 deg
/xrf/sampleSize 5 5 0.005 mm
/xrf/detectorSpreadDeg 50 deg
/xrf/detectorStepDeg 50 deg
/xrf/worldMaterial G4_AIR
/xrf/detectorThickness 0.5 mm
/xrf/setShieldEnabled false 
/xrf/setShieldGap 2 mm
/xrf/numberOfDetectorGroups 1
/xrf/clearShieldLayers
/xrf/addShieldLayer G4_Pb 1.0 mm 0.0 mm
/xrf/addShieldLayer G4_Al 0.5 mm 0.5 mm
/xrf/pileupTimeWindow {Pileup_Time_Window}
/xrf/explambda {mean_gap_s}
/xrf/outputFileName {root_folder_path}

/xrf/focalspot {focalspot} mm

/process/em/fluo true
/process/em/auger true
/process/em/pixe true
/process/em/deexcitationIgnoreCut true
/run/numberOfThreads {THREAD}

/run/initialize
/gps/particle gamma
/gps/ene/type User
/gps/hist/type energy

"""
print("fluence_at_1m =", fluence_at_1m)
print("fluence_at_source_distance =", fluence_at_detector_distance)
print("photon_per_second =", phton_per_second)
print("mean_gap_ns =", mean_gap_s)
print("photons per 100 ns =", 100 / mean_gap_s)
for e,f in zip(energy,fluence_weight):
    MACRO+=f"\n/gps/hist/point {e} {f}"
MACRO+=f"\n/run/beamOn {NUMBER_OF_EVENT}"


with open(r"../macro/run.mac",'w') as f:
    f.write(MACRO)
    
subprocess.run(['cmake',".."],cwd="../build",check=True)
subprocess.run(['make'],cwd="../build",check=True)
subprocess.run(['./sim','run.mac'],cwd="../build",check=True)


    
    