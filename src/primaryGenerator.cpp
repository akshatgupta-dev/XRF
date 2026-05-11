#include "primaryGenerator.hh"


namespace {
    G4Mutex sourceTimeMutex = G4MUTEX_INITIALIZER;
    G4double globalSourceTime = 0.0 * ns;
}
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

    G4ThreeVector beamDirection=SourceDirection.unit();
    G4ThreeVector perpVector1 = G4ThreeVector(0,1,0);
    G4ThreeVector perpVector2 = beamDirection.cross(perpVector1).unit();

    G4double focalSpotHalfSize = fConfig->focalspot / 2.0;

    G4double dV1=focalSpotHalfSize * (2.0 * G4UniformRand() - 1.0);
    G4double dV2=focalSpotHalfSize *(2*G4UniformRand()-1);

    G4ThreeVector finalSourcePosition=SourcePosition + dV1 * perpVector1 + dV2 * perpVector2;

    source->GetPosDist()->SetPosDisType("Point");
    source->GetPosDist()->SetCentreCoords(finalSourcePosition);


    G4double sampleSizeX = fConfig->sampleMaterialSize.x()/2;
    G4double sampleSizeY = fConfig->sampleMaterialSize.y()/2;

    G4double dSampleX = sampleSizeX * (2.0 * G4UniformRand() - 1.0);
    G4double dSampleY = sampleSizeY * (2.0 * G4UniformRand() - 1.0);
    G4ThreeVector sampleOffset(dSampleX, dSampleY, 0);

    G4ThreeVector finalSourceDirection = (sampleOffset-finalSourcePosition).unit();


    source->GetAngDist()->SetParticleMomentumDirection(finalSourceDirection);
    G4double proba=G4UniformRand();
    proba=std::max(proba,1e-10);
    G4double particlestarttime=0.0*ns;
    G4double dt=-std::log(proba)*fConfig->explambda;
    {
        G4AutoLock lock(&sourceTimeMutex);
        globalSourceTime+=dt;
        particlestarttime=globalSourceTime;
    }
    fgun->SetParticleTime(particlestarttime);

    fgun->GeneratePrimaryVertex(event);
G4cout << "After GeneratePrimaryVertex()" << G4endl;
G4cout << "Number of primary vertices: "
       << event->GetNumberOfPrimaryVertex() << G4endl;


    if (event->GetNumberOfPrimaryVertex() > 0) {
        G4PrimaryVertex* vertex = event->GetPrimaryVertex(0);
        G4ThreeVector actualPos = vertex->GetPosition();
        
        if (vertex->GetNumberOfParticle() > 0) {
            G4ThreeVector actualDir = vertex->GetPrimary(0)->GetMomentumDirection();
            
            // Print the results
            G4cout << "Event ID: " << event->GetEventID() << G4endl;
            G4cout << "  -> Gun Position: " << actualPos << " mm" << G4endl;
            G4cout << "  -> Gun Direction: " << actualDir << G4endl;
        }
    }
}
