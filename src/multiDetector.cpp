#include "multiDetector.hh"


G4double MultiDetectorConstruction::GetDetectorCapRadius(G4double detectorDistance, G4double spreadeg){

    return detectorDistance * std::tan(spreadeg );

}
G4double MultiDetectorConstruction::GetDetectorWidth(G4double detectorDistance, G4double detectorStepDeg){

    return 2 * detectorDistance * std::tan(detectorStepDeg  / 2);

}

G4double MultiDetectorConstruction::ComputeDetectorTheta(const G4ThreeVector& pos){

    // return std::atan2(pos.z(),pos.x())/deg;

    G4double mag=std::sqrt(pos.x()*pos.x()+pos.z()*pos.z()+pos.y()*pos.y());
    return std::acos(pos.z()/mag)/deg;

}
MultiDetectorConstruction::MultiDetectorConstruction(G4double detectorDistance, G4double takeoffAngle, G4double detectorSpreadDeg, G4double detectorStepDeg): groupAngle(takeoffAngle)
{

    capRadius=GetDetectorCapRadius(detectorDistance, detectorSpreadDeg);
    G4double width=GetDetectorWidth(detectorDistance, detectorStepDeg);

    G4ThreeVector detectorCenter(
        detectorDistance * std::cos(takeoffAngle ),
        0,
        detectorDistance * std::sin(takeoffAngle)
    );

    G4RotationMatrix rotation;
    rotation.rotateY(-(90.0 * deg + takeoffAngle));

    const G4int numDetectors=static_cast<G4int>(std::ceil(capRadius/width));


    for (int ix=-numDetectors;ix<numDetectors;++ix){

        const G4double x=ix*width;

        for (int iy=-numDetectors;iy<numDetectors;++iy){

            const G4double y=iy*width;

            if ((x*x+y*y) > capRadius*capRadius){

                continue;
            }
            G4ThreeVector localPos(x,y,0);
            G4ThreeVector globalPos=rotation*localPos+detectorCenter;

            DetectorElement element;    
            element.id = static_cast<G4int>(detectorElements.size());
            element.center=globalPos;
            element.thetadeg=ComputeDetectorTheta(globalPos);
            element.width=width;
            element.height=width;
            element.row=ix;
            element.col=iy;

            detectorElements.push_back(element);

            G4cout<<"Detector "<<element.id<<" at "<<globalPos<<" with theta "<<element.thetadeg<<" degrees"<<G4endl;

        }  
    }
    if (detectorElements.empty()){
        DetectorElement element;
        element.id=0;
        element.center=detectorCenter;
        element.thetadeg=ComputeDetectorTheta(detectorCenter);
        element.width=width;
        element.height=width;
        detectorElements.push_back(element);

    }   
}

MultiDetectorConstruction::~MultiDetectorConstruction(){


}