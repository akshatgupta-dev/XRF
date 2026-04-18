#include "runAction.hh"

RunAction::RunAction()
{
    auto* analysismanager = G4AnalysisManager::Instance();
    analysismanager->SetNtupleMerging(true);
    analysismanager->CreateH1("Energydeposit", "Energy deposit", 600, 0, 60);
    analysismanager->CreateNtuple("Photons", "Photons");
    analysismanager->CreateNtupleIColumn("IEvent");
    analysismanager->CreateNtupleDColumn("KEV_Energy");
    analysismanager->FinishNtuple(0);
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*)
{
    auto* analysismanager = G4AnalysisManager::Instance();
    analysismanager->Reset();
    analysismanager->OpenFile("output.root");
}

void RunAction::EndOfRunAction(const G4Run*)
{
    auto* analysismanager = G4AnalysisManager::Instance();
    analysismanager->Write();
    analysismanager->CloseFile(false);
}