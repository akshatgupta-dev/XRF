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
void PrimaryGenerator::GeneratePrimaries(G4Event *event)
{
    CalulateSourcePositionAndDirection();

    auto* source = fgun->GetCurrentSource();

    source->GetPosDist()->SetPosDisType("Point");
    source->GetPosDist()->SetCentreCoords(SourcePosition);
    source->GetAngDist()->SetParticleMomentumDirection(SourceDirection.unit());

    // G4cout << "SourcePosition = " << SourcePosition/mm << " mm, "
    //        << "Direction = " << SourceDirection.unit() << G4endl;

    fgun->GeneratePrimaryVertex(event);
}