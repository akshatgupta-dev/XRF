#include "trackAction.hh"
#include "trackHistory.hh"



void TrackAction::PreUserTrackingAction(const G4Track* track){

    TrackHistory::addTrack(track);

}