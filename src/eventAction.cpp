#include "eventAction.hh"
#include "trackHistory.hh"

void EventAction::BeginOfEventAction(const G4Event* event){

    TrackHistory::clearHistory();

}