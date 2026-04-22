
#include "detectorType.hh"




SensitiveDetector::SensitiveDetector(G4String name,SimulationConfig &config):G4VSensitiveDetector(name),fTotalEnergyDeposited(0.0),fConfig(&config){

}

SensitiveDetector::~SensitiveDetector(){

}   


void SensitiveDetector::Initialize(G4HCofThisEvent *hce){

    fTotalEnergyDeposited=0.0;
    fDetectorEnergyMap.clear();

}

G4bool SensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *hist)
{
    G4double edep = step->GetTotalEnergyDeposit();

    if (edep <= 0) {
        return true; 
    }
    auto touchable=step->GetPreStepPoint()->GetTouchableHandle();
    G4int detId=touchable->GetCopyNumber();
    fDetectorEnergyMap[detId] += edep;


    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent *hce){

    for (const auto& entry : fDetectorEnergyMap) {
        G4int detId = entry.first;
        G4double energy = entry.second;

        auto* analysisManager = G4AnalysisManager::Instance();

        analysisManager->FillH1(detId+1, energy / keV);
        analysisManager->FillNtupleIColumn(1,0, detId);
        analysisManager->FillNtupleDColumn(1,1, energy / keV);
        analysisManager->AddNtupleRow(1);
    }
}
