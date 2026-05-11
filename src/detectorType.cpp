
#include "detectorType.hh"

#include "trackHistory.hh"


SensitiveDetector::SensitiveDetector(G4String name,SimulationConfig &config,std::vector<VirtualDetector>& virtualDetectors):G4VSensitiveDetector(name),fTotalEnergyDeposited(0.0),fConfig(&config),allvirtualDetectors(virtualDetectors){

}

SensitiveDetector::~SensitiveDetector(){

}   

void SensitiveDetector::Initialize(G4HCofThisEvent *hce){

    fTotalEnergyDeposited=0.0;
    recordingInfos.clear();
    fDetectorPhotonRootId.clear();

}

G4int SensitiveDetector::GetDetectorPhotonRootId(G4int detId,const G4Track *track){
    auto &rootMap=fDetectorPhotonRootId[detId];
    G4int originalTrackId=track->GetTrackID();
    G4int currentTrackId=originalTrackId;
    std::vector<G4int>visitedTracks;
    while(currentTrackId>0){
        visitedTracks.push_back(currentTrackId);
        auto it=rootMap.find(currentTrackId);

        if(it!=rootMap.end()){
            G4int rootId=it->second;
            for(const auto& visitedTrackId:visitedTracks){
                rootMap[visitedTrackId]=rootId;

            }
            return rootId;
        }
        currentTrackId=TrackHistory::getParentId(currentTrackId);
    }

    rootMap[originalTrackId]=originalTrackId;
    return originalTrackId;


}


G4bool SensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *hist)
{
    G4double edep = step->GetTotalEnergyDeposit();

    G4int trackId = step->GetTrack()->GetTrackID();

    G4double time = step->GetPreStepPoint()->GetGlobalTime();
    auto touchable=step->GetPreStepPoint()->GetTouchableHandle();
    G4int detId=touchable->GetCopyNumber();
    G4int ascId=GetDetectorPhotonRootId(detId,step->GetTrack());
    auto& trackMap = recordingInfos[detId];
    if (edep <= 0) {
            return true; 
        }

    if (trackMap.find(ascId) == trackMap.end()) {
        trackMap[ascId] = RecordingInfo(detId, edep, ascId, time);
    }

    else {
        trackMap[ascId].energy += edep;
    }
    return true;
}
void SensitiveDetector::EndOfEvent(G4HCofThisEvent *hce){

auto* analysisManager = G4AnalysisManager::Instance();

    for (const auto& [detId, trackMap] : recordingInfos) {
        G4double totalDetectorEnergy = 0.0;
        
        for (const auto& [trackId, info] : trackMap) {
             

            if (info.energy > 0) {
                analysisManager->FillH1(detId + 1, info.energy / keV);
                
                analysisManager->FillNtupleIColumn(1, 0, detId);
                analysisManager->FillNtupleDColumn(1, 2, info.energy/ keV); 
                analysisManager->FillNtupleDColumn(1, 3, info.arivedTime/ns); 
                analysisManager->AddNtupleRow(1);
        }

        }
    
    }

    for(const auto& virtualDet:allvirtualDetectors){
        G4int detId=virtualDet.copyNumbers;
        for(const auto & copyNum:virtualDet.copyNumbersVec){
            auto it=recordingInfos.find(copyNum);
            if(it!=recordingInfos.end()){
                const auto &trackMap=it->second;

                for (const auto& [trackId, info] : trackMap) {
             

                        if (info.energy > 0) {
                            analysisManager->FillH1(detId + 1, info.energy / keV);
                            
                            analysisManager->FillNtupleIColumn(1, 0, detId);
                            analysisManager->FillNtupleDColumn(1, 2, info.energy/ keV); 
                            analysisManager->FillNtupleDColumn(1, 3, info.arivedTime/ns); 
                            analysisManager->AddNtupleRow(1);
                         }
                    }
            }
        }
    }
}
