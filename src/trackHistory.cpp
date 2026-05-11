#include "trackHistory.hh"

thread_local std::unordered_map<G4int,TrackNode> TrackHistory::trackhistory;

void TrackHistory::clearHistory(){
    trackhistory.clear();
}

void TrackHistory::addTrack(const G4Track *track){

    trackhistory[track->GetTrackID()]=TrackNode{track->GetParentID(),track->GetDefinition()->GetParticleName()};
}

G4int TrackHistory::getParentId(G4int trackId){

    auto it=trackhistory.find(trackId);
    if(it==trackhistory.end()){
        return -1;
    }
    return it->second.parentId;

}
G4String TrackHistory::getParticleName(G4int trackId){
    auto it=trackhistory.find(trackId);
    if(it==trackhistory.end()){
        return "Unknown";
    }
    return it->second.particleName;
}