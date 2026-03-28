#include "PMSensitiveDetector.hh"

#include "G4Step.hh"
#include "G4ios.hh"

PMSensitiveDetector::PMSensitiveDetector(G4String name)
    : G4VSensitiveDetector(name),
      fTotalEnergyDeposited(0.)
{
}

PMSensitiveDetector::~PMSensitiveDetector()
{
}

void PMSensitiveDetector::Initialize(G4HCofThisEvent*)
{
    fTotalEnergyDeposited = 0.;
}

void PMSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    G4cout << "Deposited Energy: " << fTotalEnergyDeposited / keV << " keV" << G4endl;
}

G4bool PMSensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
    G4double fEnergyDeposited = aStep->GetTotalEnergyDeposit();

    if (fEnergyDeposited > 0.)
    {
        fTotalEnergyDeposited += fEnergyDeposited;
    }

    return true;
}