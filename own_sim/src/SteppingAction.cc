#include "SteppingAction.hh"
#include "RunAction.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VPhysicalVolume.hh"
#include "G4StepStatus.hh"

#include <cctype>
#include <string>

SteppingAction::SteppingAction(RunAction* runAction)
    : fRunAction(runAction)
{
}

G4int SteppingAction::ExtractLayerIndex(const G4String& name) const
{
    // expects Shield_0_Back_PV etc.
    std::string s = name;
    const std::string key = "Shield_";

    std::size_t pos = s.find(key);
    if (pos == std::string::npos) return -1;

    pos += key.size();

    std::string digits;
    while (pos < s.size() && std::isdigit(static_cast<unsigned char>(s[pos]))) {
        digits += s[pos];
        ++pos;
    }

    if (digits.empty()) return -1;
    return std::stoi(digits);
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    if (!fRunAction || !step) return;

    auto* post = step->GetPostStepPoint();
    if (!post) return;

    auto* pre = step->GetPreStepPoint();
if (!pre) return;

auto* prePhys = pre->GetPhysicalVolume();
if (!prePhys) return;

const G4String pvName = prePhys->GetName();

if (pvName.find("Shield_") == std::string::npos) return;

const G4int layer = ExtractLayerIndex(pvName);
if (layer < 0) return;

fRunAction->RecordShieldLayerEntry(layer, step->GetTotalEnergyDeposit());
}