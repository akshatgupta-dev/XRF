#include "actionInitialization.hh"


ActionInitialization::~ActionInitialization(){}

void ActionInitialization::Build() const{

    SetUserAction(new PrimaryGenerator(*fConfig));
    SetUserAction(new RunAction());

}
void ActionInitialization::BuildForMaster() const{

    SetUserAction(new RunAction());

}