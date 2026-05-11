#include "actionInitialization.hh"
#include "steppingAction.hh"
#include "trackAction.hh"
#include "eventAction.hh"

ActionInitialization::~ActionInitialization(){}

void ActionInitialization::Build() const{

    SetUserAction(new PrimaryGenerator(*fConfig));
    SetUserAction(new RunAction(*fConfig));
    SetUserAction(new SteppingAction());
    SetUserAction(new TrackAction());
    SetUserAction(new EventAction());

}
void ActionInitialization::BuildForMaster() const{

    SetUserAction(new RunAction(*fConfig));

}