#include "detectorConstruction.hh"
#include "multiDetector.hh"

#include "detectorType.hh"


DetectorConstruction::DetectorConstruction(SimulationConfig& config):fConfig(&config){}




G4VPhysicalVolume* DetectorConstruction::Construct(){

    allDetectors.clear();

    G4double takeoffangle=fConfig->takeoffAngle;

    int buildMultiDetector=0;
    G4double currenttakeoffangle=takeoffangle;
    G4double spreaddeg=fConfig->detectorSpreadDeg;


    
    std::vector<G4double> detectorangles;
    detectorangles.push_back(takeoffangle);

    G4double upperanglelimit=180*deg;
    G4double loweranglelimit=0*deg;
    G4int maxdetectors=fConfig->numDetectorGroups;

    G4int upperCounter=0;
    G4int lowerCounter=0;
    G4double offset=spreaddeg+7*deg;

    for (int i=1;detectorangles.size()<maxdetectors;++i){

        G4double upperangle=currenttakeoffangle+i*offset;
        G4double lowerangle=currenttakeoffangle-i*offset;

        if(upperangle<upperanglelimit && upperCounter<maxdetectors/2){

            detectorangles.push_back(upperangle);
            upperCounter++;
        }

        else if(lowerangle>loweranglelimit && lowerCounter<maxdetectors/2){

            detectorangles.push_back(lowerangle);
            lowerCounter++;
        }

        if(detectorangles.size()>=maxdetectors){

            break;
        }

        if (lowerangle > loweranglelimit){
                    detectorangles.push_back(lowerangle);
                    lowerCounter++;

        }

        if (detectorangles.size() >= maxdetectors) break;


    }


    for(G4double angle:detectorangles){

        MultiDetectorConstruction multiDetector(fConfig->detectorDistance,angle,fConfig->detectorSpreadDeg,fConfig->detectorStepDeg);
        G4cout << "Created multi-detector with group angle: " << angle/deg << " degrees, number of elements: " << multiDetector.detectorElements.size() << G4endl;

        allDetectors.emplace_back(multiDetector);
    }

    

    auto *nist=G4NistManager::Instance();

    auto *worldMat=nist->FindOrBuildMaterial(fConfig->worldMaterial);

    G4Material *sampleMat=nullptr;


    if (fConfig->sampleMaterialIsCustom && !fConfig->sampleMaterialComponents.empty() && !sampleMat){

        sampleMat=new G4Material("samplemat",fConfig->sampleMaterialDensity,fConfig->sampleMaterialComponents.size());


        for(const auto& component:fConfig->sampleMaterialComponents){

            G4Element *element=nist->FindOrBuildElement(component.element);

            if(element){
                sampleMat->AddElement(element,component.fraction);
            }
        }


    }

    if(!sampleMat){

        sampleMat=nist->FindOrBuildMaterial(fConfig->sampleMaterial);
    }





    auto *worldSolid=new G4Box("worldSolid",15*cm,15*cm,15*cm);

    auto *worldLogic=new G4LogicalVolume(worldSolid,worldMat,"worldLogic");

    auto *worldPhys=new G4PVPlacement(nullptr,G4ThreeVector(),worldLogic,"worldPhys",nullptr,false,0,true);

    auto *sampleSolid=new G4Box("sampleSolid",fConfig->sampleMaterialSize.x()/2,fConfig->sampleMaterialSize.y()/2,fConfig->sampleMaterialSize.z()/2);
    auto *sampleLogic=new G4LogicalVolume(sampleSolid,sampleMat,"sampleLogic");
    auto *samplePhys=new G4PVPlacement(nullptr,G4ThreeVector(0,0,0),sampleLogic,"samplePhys",worldLogic,false,1,true);



    auto *detectorMat=nist->FindOrBuildMaterial("G4_Si");

    const G4double detectorThickness=fConfig->detectorthickness;

    const G4double detectorwidth=allDetectors.front().detectorElements.front().width;
    G4cout<<"Detector width: "<<detectorwidth<<" mm"<<G4endl;    

    auto *detectorSolid=new G4Box("detectorSolid",detectorwidth/2,detectorwidth/2,detectorThickness/2);

    fDetectorLV=new G4LogicalVolume(detectorSolid,detectorMat,"detectorLogic");
  auto* detVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  detVis->SetVisibility(true);
  fDetectorLV->SetVisAttributes(detVis);


    int copynumber=0;
    for (const auto& multiDetector:allDetectors){

            G4RotationMatrix rotation;
            rotation.rotateY(-(90*deg+multiDetector.groupAngle));

            G4RotationMatrix inverseRotation=rotation.inverse();

        for (const auto& element:multiDetector.detectorElements){
            G4String detName = "detectorPhys_g" + std::to_string(copynumber);

            
            G4cout << "Placing detector element ID: " << element.id 
                   << " at position: " << element.center 
                   << " with theta: " << element.thetadeg 
                   << " degrees, width: " << element.width 
                   << " mm, height: " << element.height 
                   << " mm" << G4endl;
            new G4PVPlacement(new G4RotationMatrix(inverseRotation),element.center,fDetectorLV,detName,worldLogic,false,element.id,true);
            copynumber++;
        
        }
    }

    G4cout<<"sample material: "<<sampleMat->GetName()<<G4endl;
    G4cout<<"world material: "<<worldMat->GetName()<<G4endl;   

    return worldPhys;

    
}

DetectorConstruction::~DetectorConstruction() = default;



void DetectorConstruction::ConstructSDandField(){
    auto *sd=new SensitiveDetector("XRF_SensitiveDetector",*fConfig);
    G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    fDetectorLV->SetSensitiveDetector(sd);

}