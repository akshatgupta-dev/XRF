import uproot
import h5py
import os 
import numpy as np


def create_pileup_and_add_nois(file_path,pile_up_window_time):
    file=uproot.open(file_path)
    hits_df=file['hits;1'].arrays(library='pd')

    unique_det=hits_df['detID'].unique()

    for det in unique_det:
        
        tempo_df=hits_df[hits_df['detID']==det]

        print(tempo_df)
        print(tempo_df['time_ns'].diff()<100)
        
        


if __name__=="__main__":
    create_pileup_and_add_nois(r"/home/minmyatngwe/geant_4_xrf/XRF_SIMULATION/output_file/FE_100/SECOND_test.root",100)
    
    
    
    
    
        
        
        
        

    
    
    