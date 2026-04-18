#ifndef MULTIDETECTOR_HH
#define MULTIDETECTOR_HH
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "detectorElement.hh"
#include "G4RotationMatrix.hh"
#include <cmath>
#include <vector>
class MultiDetectorConstruction{
    public:
        MultiDetectorConstruction(G4double detectorDistance, G4double takeoffAngle, G4double detectorSpreadDeg, G4double detectorStepDeg);
        ~MultiDetectorConstruction();
        G4double GetDetectorCapRadius(G4double detectorDistance,G4double spreadeg);
        G4double GetDetectorWidth(G4double detectorDistance, G4double detectorStepDeg);
        G4double ComputeDetectorTheta(const G4ThreeVector& pos);
        std::vector<DetectorElement> detectorElements;
        G4double groupAngle;
        G4double capRadius;

        
    
};


#endif 