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
    auto &shield=fConfig->shield;
for (const auto& multiDetector : allDetectors) {
        
        G4RotationMatrix rotation;
        rotation.rotateY(-(90 * deg + multiDetector.groupAngle));

        G4RotationMatrix inverseRotation = rotation.inverse();

        for (const auto& element : multiDetector.detectorElements) {
            G4String detName = "detectorPhys_g" + std::to_string(copynumber);
            
            new G4PVPlacement(new G4RotationMatrix(inverseRotation), element.center, fDetectorLV, detName, worldLogic, false, element.id, true);
            copynumber++;
        }

        if (shield.enable && shield.layers.size() > 0) {
            G4double capRadius = multiDetector.capRadius;

            const G4double gap = shield.detectorAndFirstLayerGap;
            const G4double innerHalfX = capRadius + 0.5 * detectorwidth + gap;
            const G4double innerHalfY = capRadius + 0.5 * detectorwidth + gap;

            G4ThreeVector detectorcenter(
                std::cos(multiDetector.groupAngle) * fConfig->detectorDistance,
                0,
                std::sin(multiDetector.groupAngle) * fConfig->detectorDistance
            );

            G4double currentHalfX = innerHalfX;
            G4double currentHalfY = innerHalfY;
            G4double currentBack  = gap;

            for (size_t i = 0; i < shield.layers.size(); ++i) {

                const auto& layer = shield.layers[i];
                G4Material* layerMat = nist->FindOrBuildMaterial(layer.material);
                
                const G4double currentGap = std::max(0.0, layer.gapbefore); // Renamed to avoid shadowing outer 'gap'
                const G4double t   = layer.thickness;

                currentHalfX += currentGap;
                currentHalfY += currentGap;
                currentBack  += currentGap;
                
                const G4double outerHalfX = currentHalfX + t;
                const G4double outerHalfY = currentHalfY + t;
                const G4double outerBack  = currentBack  + t;

                const G4String base = "Shield_" + std::to_string(i);

                auto* outerSolid = new G4Box(base + "_Outer_Solid",
                     outerHalfX,
                     outerHalfY,
                     (currentBack + t) / 2.0);
                
                auto* innerSolid = new G4Box(base + "_Inner_Solid",
                     currentHalfX,
                     currentHalfY,
                     currentBack / 2.0);

                auto* shieldSolid = new G4SubtractionSolid(
                    base + "_Shell_Solid",
                    outerSolid,
                    innerSolid,
                    nullptr,
                    G4ThreeVector(0.0, 0.0, t / 2.0)
                );

                auto* logic = new G4LogicalVolume(shieldSolid, layerMat, base + "_LV");
                auto* shieldVis = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.4)); 
                shieldVis->SetForceSolid(true);
                logic->SetVisAttributes(shieldVis);
                G4ThreeVector localPos(
                    0.0,
                    0.0,
                    -(fConfig->detectorthickness + (currentBack + t) / 2.0)
                );

                G4ThreeVector globalPos = detectorcenter + rotation * localPos;

                new G4PVPlacement(new G4RotationMatrix(inverseRotation),
                        globalPos,
                        logic,
                        base + "_PV",
                        worldLogic,
                        false,
                        static_cast<G4int>(1000 + i),
                        true);

                currentHalfX = outerHalfX;
                currentHalfY = outerHalfY;
                currentBack  = outerBack;
            } // Ends layer loop
        } // Ends if(shield.enable) statement
        
        // FIX: Removed the extra closing brace that was floating here
    } // Ends allDetectors loop

    return worldPhys;
}

DetectorConstruction::~DetectorConstruction() = default;

void DetectorConstruction::ConstructSDandField(){
    auto *sd=new SensitiveDetector("XRF_SensitiveDetector",*fConfig);
    G4SDManager::GetSDMpointer()->AddNewDetector(sd);
    fDetectorLV->SetSensitiveDetector(sd);
}