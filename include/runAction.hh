#ifndef RUNCATION_HH
#define RUNCATION_HH


#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "detectorConstruction.hh"

class RunAction:public G4UserRunAction{

    public:
        RunAction();
        ~RunAction() override;
        void BeginOfRunAction(const G4Run* run) override;
        void EndOfRunAction(const G4Run* run) override;

        void CreateDedectorHistogram(G4int nDetectors);
        void FillDetectorHistogram(G4int detId,G4double energy);

        void FillDetectorMetadata(
            G4int detId,
            G4String material,
            G4double samplesizeX,
            G4double samplesizeY,
            G4double samplesize,
            G4double incidentAngle,
            G4double sourceDistance,
            G4double detectorDistance,
            G4double takeoffAngle,
            G4double detectorLocationX,
            G4double detectorLocationY,
            G4double detectorLocationZ,
            G4double detectorWidth,
            G4double detectorThickness,
            G4double detectorHeight,
            G4String worldMat,
            G4String detectorType
        ) const;
    private:


};

#endif