#ifndef DETECTORELEMENT_HH
#define DETECTORELEMENT_HH

#include "G4ThreeVector.hh"
#include "globals.hh"
#include "vector"
struct DetectorElement {
    G4int id;
    G4ThreeVector center;
    G4int row;
    G4int col;
    G4double thetadeg;
    G4double width;
    G4double height;
    G4int copynumber;
    G4int groupId;
};

struct VirtualDetector{

    G4ThreeVector center;
    G4double width, height, thickness;
    G4double thetadeg;
    G4double incidentAngle;
    std::vector <G4int> copyNumbersVec;
    G4int copyNumbers;


};

#endif