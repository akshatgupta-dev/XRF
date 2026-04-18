#ifndef RUNCATION_HH
#define RUNCATION_HH


#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"


class RunAction:public G4UserRunAction{

    public:
        RunAction();
        ~RunAction() override;
        void BeginOfRunAction(const G4Run* run) override;
        void EndOfRunAction(const G4Run* run) override;


};

#endif