#ifndef TRACKACTION_HH
#define TRACKACTION_HH


#include "G4UserTrackingAction.hh"
#include "G4Track.hh"

class TrackAction:public G4UserTrackingAction
{
    public:
        TrackAction()=default;
        ~TrackAction() override=default;
        void PreUserTrackingAction(const G4Track* track) override;

};


#endif