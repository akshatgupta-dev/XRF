#include "detectorConstruction.hh"
#include "multiDetector.hh"

#include "detectorType.hh"

DetectorConstruction::DetectorConstruction(SimulationConfig& config):fConfig(&config){
    G4cout << "Initial shield.enable = " << std::boolalpha << fConfig->shield.enable << G4endl;
}

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
    G4double offset=(2*spreaddeg)+50*deg;

    if (maxdetectors>1){
        G4int trial=1000;
        for (int i=1;i<trial;++i){

        G4double upperangle=currenttakeoffangle+i*offset;
        G4double lowerangle=currenttakeoffangle-i*offset;
        G4cout<<"Iteration "<<i<<": upperangle="<<upperangle/deg<<" deg, lowerangle="<<lowerangle/deg<<" deg"<<G4endl;
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

    }


    }


    for(G4double angle:detectorangles){

        MultiDetectorConstruction multiDetector(fConfig->detectorDistance,angle,fConfig->detectorSpreadDeg,fConfig->detectorStepDeg);
        G4cout << "Created multi-detector with group angle: " << angle/deg << " degrees, number of elements: " << multiDetector.detectorElements.size() << G4endl;
        allDetectors.emplace_back(multiDetector);
    }

    

    auto *nist=G4NistManager::Instance();

    auto *worldMat=nist->FindOrBuildMaterial(fConfig->worldMaterial);

    G4Material *sampleMat=nullptr;

    G4String sampleMatString;

    if (fConfig->sampleMaterialIsCustom && !fConfig->sampleMaterialComponents.empty() && !sampleMat){

        sampleMat=new G4Material("samplemat",fConfig->sampleMaterialDensity,fConfig->sampleMaterialComponents.size());
        for(const auto& component:fConfig->sampleMaterialComponents){
            G4Element *element=nist->FindOrBuildElement(component.element);
            if(element){
                sampleMat->AddElement(element,component.fraction);
                sampleMatString+=component.element+" "+std::to_string(component.fraction)+" ";
            }
        }
    }
    if(!sampleMat){

        sampleMat=nist->FindOrBuildMaterial(fConfig->sampleMaterial);
        sampleMatString=fConfig->sampleMaterial;
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
G4cout << "Sample material = "
       << sampleLogic->GetMaterial()->GetName()
       << G4endl;

    auto &shield=fConfig->shield;

    
    std::map<int,std::map<int,std::vector<DetectorElement>>>rowmap;
    std::map<int,std::map<int,std::vector<DetectorElement>>>columnmap;



    G4int copynumber=0;
    G4int groupId=0;
    for (const auto& multiDetector : allDetectors) {
            
            G4RotationMatrix rotation;
            rotation.rotateY(-(90 * deg + multiDetector.groupAngle));
            G4RotationMatrix inverseRotation = rotation.inverse();
            for (const auto& element : multiDetector.detectorElements) {
                G4String detName = "detectorPhys_g" + std::to_string(element.id);

                rowmap[groupId][element.row].push_back(element);
                columnmap[groupId][element.col].push_back(element);
                
                // element.copynumber=copynumber;
                // element.groupId=groupId;
                DetectorMeta meta(
                    copynumber,
                    sampleMatString,
                    fConfig->sampleMaterialSize.x(),
                    fConfig->sampleMaterialSize.y(),
                    fConfig->sampleMaterialSize.z(),
                    fConfig->incidentAngle,
                    fConfig->sourceDistance,
                    fConfig->detectorDistance,
                    element.thetadeg,
                    element.center.x(),
                    element.center.y(),
                    element.center.z(),
                    element.width,
                    detectorThickness,
                    element.width,
                    fConfig->worldMaterial,
                    fConfig->detectorType
                );
                G4cout<<"Adding detector metadata for detector ID " << meta.detId << ": "
                    <<element.thetadeg<< G4endl;
                detectorMetadata.emplace_back(meta);
                new G4PVPlacement(new G4RotationMatrix(inverseRotation), element.center, fDetectorLV, detName, worldLogic, false, copynumber, true);
                copynumber++;
            }
                
    if (shield.enable && !shield.layers.empty()) {

        const G4double capRadius = multiDetector.capRadius;
        const G4double detHalfZ  = detectorThickness/2;

        G4ThreeVector detectorcenter(
            std::cos(multiDetector.groupAngle) * fConfig->detectorDistance,
            0.0,
            std::sin(multiDetector.groupAngle) * fConfig->detectorDistance
        );

        G4double currentHalfX = capRadius+detectorwidth/2;
        G4double currentHalfY = capRadius+detectorwidth/2;
        G4double currentHalfZ = detHalfZ;

        G4double currentCenterShiftZ = 0.0;

        const G4int shieldCopyBase = 100000 + copynumber * 100;

        for (size_t i = 0; i < shield.layers.size(); ++i) {

            const auto& layer = shield.layers[i];
            const G4double gap = std::max(0.001*mm, layer.gapbefore);
            const G4double wall = layer.thickness;

            const G4double innerHalfX = currentHalfX + gap;
            const G4double innerHalfY = currentHalfY + gap;
            const G4double innerHalfZ = currentHalfZ + gap;

            const G4double outerHalfX = innerHalfX + wall;
            const G4double outerHalfY = innerHalfY + wall;
            const G4double outerHalfZ = innerHalfZ + wall / 2.0;

            const G4double cavityShiftZ = outerHalfZ - innerHalfZ;

            const G4double shellCenterShiftZ = currentCenterShiftZ -cavityShiftZ;

            const G4String base =
                "Shield_g" + std::to_string(shieldCopyBase) + "_L" + std::to_string(i);

            auto* outerSolid = new G4Box(
                base + "_Outer_Solid",
                outerHalfX,
                outerHalfY,
                outerHalfZ
            );

            auto* innerSolid = new G4Box(
                base + "_Inner_Solid",
                innerHalfX,
                innerHalfY,
                innerHalfZ
            );

            auto* shieldSolid = new G4SubtractionSolid(
                base + "_Shell_Solid",
                outerSolid,
                innerSolid,
                nullptr,
                G4ThreeVector(0.0, 0.0, cavityShiftZ)
            );

            G4Material* layerMat = nist->FindOrBuildMaterial(layer.material);

            auto* logic = new G4LogicalVolume(
                shieldSolid,
                layerMat,
                base + "_LV"
            );

            auto* shieldVis = new G4VisAttributes(
                (i == 0) ? G4Colour(1, 0, 0, 1)
                        : G4Colour(0.7, 0.2, 0.2, 0.5)
            );
            shieldVis->SetForceSolid(true);
            logic->SetVisAttributes(shieldVis);

            G4ThreeVector localPos(0.0, 0.0, shellCenterShiftZ);
            G4ThreeVector globalPos = detectorcenter + rotation * localPos;

            G4cout << "Placing shield layer " << i
                << " at global position: " << globalPos / mm << " mm"
                << "  innerHalfZ=" << innerHalfZ / mm
                << " mm  outerHalfZ=" << outerHalfZ / mm
                << " mm  cavityShiftZ=" << cavityShiftZ / mm
                << " mm" << G4endl;

            new G4PVPlacement(
                new G4RotationMatrix(inverseRotation),
                globalPos,
                logic,
                base + "_PV",
                worldLogic,
                false,
                shieldCopyBase + static_cast<G4int>(i),
                true
            );

            currentHalfX = outerHalfX;
            currentHalfY = outerHalfY;
            currentHalfZ = outerHalfZ;
            currentCenterShiftZ = shellCenterShiftZ;
        }
        }    
        
        groupId++;
    } 

    // for(auto & [groupId,row]:rowmap){
    //     for(auto &[rowNum,detectorvec]:row){
    //         sortDetectorsRow(detectorvec);

    //         for(int start=0;start<detectorvec.size();++start){


    //         }

            
            

    //     }


    // }



    return worldPhys;
}

DetectorConstruction::~DetectorConstruction() = default;

void DetectorConstruction::ConstructSDandField(){
    auto *sd=new SensitiveDetector("XRF_SensitiveDetector",*fConfig);
    G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    fDetectorLV->SetSensitiveDetector(sd);
}

void DetectorConstruction::sortDetectorsRow(std::vector<DetectorElement>& detectors){

    std::sort(detectors.begin(),detectors.end(),[](const DetectorElement& a,const DetectorElement& b){

        return a.center.y()<b.center.y();

    });

}