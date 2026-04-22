#include "actionInitialization.hh"
#include "steppingAction.hh"


ActionInitialization::~ActionInitialization(){}

void ActionInitialization::Build() const{

    SetUserAction(new PrimaryGenerator(*fConfig));
    SetUserAction(new RunAction());
        SetUserAction(new SteppingAction());


}
void ActionInitialization::BuildForMaster() const{

    SetUserAction(new RunAction());

}