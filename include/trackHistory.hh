#ifndef TRACKHISTORY_HH
#define TRACKHISTORY_HH

#include "G4Track.hh"
#include "globals.hh"
#include "G4String.hh"
#include <unordered_map>
struct TrackNode{

    G4int parentId;
    G4String particleName;
};

class TrackHistory{
    public:


    static void clearHistory();
    
    static void addTrack(const  G4Track* track);

    static G4int getParentId(G4int trackId);
    static G4String getParticleName(G4int trackId);

    private:
        static thread_local std::unordered_map<G4int,TrackNode> trackhistory;

};

#endif