#include "runAction.hh"
#include "G4Threading.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"

G4bool RunAction::fMetadataWritten = false;
G4Mutex RunAction::fMetadataMutex = G4MUTEX_INITIALIZER;
RunAction::RunAction()
{
    auto* analysismanager = G4AnalysisManager::Instance();
    analysismanager->SetNtupleMerging(true);
    analysismanager->CreateNtuple("detectorMetata", "Metadata for each detector");
    analysismanager->CreateNtupleIColumn("DetectorId");
    analysismanager->CreateNtupleSColumn("MaterialComposition");
    analysismanager->CreateNtupleDColumn("SampleSizeWidht_mm");
    analysismanager->CreateNtupleDColumn("SampleSizeHeight_mm");
    analysismanager->CreateNtupleDColumn("SampleSizeThickness_mm");
    analysismanager->CreateNtupleDColumn("IncidentAngle_deg");
    analysismanager->CreateNtupleDColumn("SourceDistance_mm");
    analysismanager->CreateNtupleDColumn("DetectorDistance_mm");
    analysismanager->CreateNtupleDColumn("TakeoffAngle_deg");
    analysismanager->CreateNtupleDColumn("DetectorLocationX_mm");
    analysismanager->CreateNtupleDColumn("DetectorLocationY_mm");
    analysismanager->CreateNtupleDColumn("DetectorLocationZ_mm");
    analysismanager->CreateNtupleDColumn("DetectorWidth_mm");
    analysismanager->CreateNtupleDColumn("DetectorThickness_mm");
    analysismanager->CreateNtupleDColumn("DetectorHeight_mm");
    analysismanager->CreateNtupleSColumn("WorldMaterial");
    analysismanager->CreateNtupleSColumn("DetectorType");

    analysismanager->FinishNtuple(0);



    analysismanager->CreateNtuple("hits", "Raw detector hits");

    analysismanager->CreateNtupleIColumn("eventID");
    analysismanager->CreateNtupleIColumn("detID");
    analysismanager->CreateNtupleDColumn("energy");

    analysismanager->FinishNtuple(1); 

    analysismanager->CreateNtuple("shieldInformation", "Information about shield layers");
    analysismanager->CreateNtupleIColumn("layerID");
    analysismanager->CreateNtupleSColumn("material");
    analysismanager->CreateNtupleDColumn("thickness_mm");
    analysismanager->CreateNtupleDColumn("gapBefore_mm");
    analysismanager->FinishNtuple(2);
}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*)
{
    auto* analysismanager = G4AnalysisManager::Instance();
    G4cout << "Opening output file: output.root" << G4endl;
    analysismanager->OpenFile("output.root");

    if (G4Threading::IsMasterThread()) return;

    G4AutoLock lock(&fMetadataMutex);

    if (fMetadataWritten) return;

    auto* detConst = static_cast<const DetectorConstruction*>(
        G4RunManager::GetRunManager()->GetUserDetectorConstruction()
    );

    const auto& metadata = detConst->GetDetectorMetadata();

    for (const auto& meta : metadata) {
        FillDetectorMetadata(
            meta.detId,
            meta.material,
            meta.samplewidth,
            meta.sampleheight,
            meta.samplethickness,
            meta.incidentAngle,
            meta.sourceDistance,
            meta.detectorDistance,
            meta.takeoffAngle,
            meta.detectorx,
            meta.detectory,
            meta.detectorz,
            meta.width,
            meta.thickness,
            meta.height,
            meta.worldMat,
            meta.detectorType
        );
    }

    fMetadataWritten = true;
}

void RunAction::EndOfRunAction(const G4Run*)
{
    auto* analysismanager = G4AnalysisManager::Instance();
    analysismanager->Write();
    analysismanager->CloseFile(false);
}

void RunAction::CreateDedectorHistogram(G4int nDetectors){

    auto* analysisManager = G4AnalysisManager::Instance();

    for(G4int i=0;i<nDetectors;i++){
        analysisManager->CreateH1("det_"+std::to_string(i)+"_spectrum","Energy spectrum for detector "+std::to_string(i),5000,0,120);
    }

}

void RunAction::FillDetectorMetadata(G4int detId, G4String material, G4double samplesizeX, G4double samplesizeY, G4double samplesizeZ, G4double incidentAngle, G4double sourceDistance, G4double detectorDistance, G4double takeoffAngle, G4double detectorLocationX, G4double detectorLocationY, G4double detectorLocationZ, G4double detectorWidth, G4double detectorThickness, G4double detectorHeight, G4String worldMat, G4String detectorType)const{

    auto* analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleIColumn(0,0, detId);
    analysisManager->FillNtupleSColumn(0,1, material);
    analysisManager->FillNtupleDColumn(0,2, samplesizeX);
    analysisManager->FillNtupleDColumn(0,3, samplesizeY);
    analysisManager->FillNtupleDColumn(0,4, samplesizeZ);
    analysisManager->FillNtupleDColumn(0,5, incidentAngle);
    analysisManager->FillNtupleDColumn(0,6, sourceDistance);
    analysisManager->FillNtupleDColumn(0,7, detectorDistance);
    analysisManager->FillNtupleDColumn(0,8, takeoffAngle);
    analysisManager->FillNtupleDColumn(0, 9, detectorLocationX);
    analysisManager->FillNtupleDColumn(0,10, detectorLocationY);
    analysisManager->FillNtupleDColumn(0,11, detectorLocationZ);
    analysisManager->FillNtupleDColumn(0,12, detectorWidth);
    analysisManager->FillNtupleDColumn(0,13, detectorThickness);
    analysisManager->FillNtupleDColumn(0,14, detectorHeight);
    analysisManager->FillNtupleSColumn(0,15, worldMat);
    analysisManager->FillNtupleSColumn(0,16, detectorType);

    analysisManager->AddNtupleRow(0);

}

