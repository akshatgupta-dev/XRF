#ifndef DETECTORELEMENT_HH
#define DETECTORELEMENT_HH

#include "G4ThreeVector.hh"
#include "globals.hh"

struct DetectorElement {
    G4int id;
    G4ThreeVector center;
    G4double thetadeg;
    G4double width;
    G4double height;
};

#endif