#include "primaryGenerator.hh"


PrimaryGenerator::PrimaryGenerator(SimulationConfig& config):fConfig(&config){

    fgun=new G4GeneralParticleSource();
}

PrimaryGenerator::~PrimaryGenerator(){
    delete fgun;
}


void PrimaryGenerator::CalulateSourcePositionAndDirection(){
    G4double theta = fConfig->incidentAngle;
    G4double distance=fConfig->sourceDistance;

    SourceDirection=G4ThreeVector(-distance*std::cos(theta),
                                     0,
                        distance*std::sin(theta)) ;

    SourcePosition = -SourceDirection;
}

void PrimaryGenerator::GeneratePrimaries(G4Event *event){
    CalulateSourcePositionAndDirection();

    fgun->GetCurrentSource()->SetParticlePosition(SourcePosition);
    fgun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(SourceDirection.unit());


    fgun->GeneratePrimaryVertex(event);
}