#include "stepping.hh"

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
{
    fEventAction = eventAction;
}

MySteppingAction::~MySteppingAction()
{}

void MySteppingAction::UserSteppingAction(const G4Step *step)
{   
    for (G4int i = 0; i < 100; ++i)
    {
        if (!fScoringVolume[i])
        {
            const auto detConstruction = static_cast<const MyDetectorConstruction*>(
            G4RunManager::GetRunManager()->GetUserDetectorConstruction());
            fScoringVolume[i] = detConstruction->GetScoringVolume(i);
        }
        else
        {
            break;
        }
    }

    
    // get volume of the current step
    G4LogicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
    G4Track* track = step->GetTrack();

    const G4StepPoint* preStepPoint = step->GetPreStepPoint();
    const G4StepPoint* postStepPoint = step->GetPostStepPoint();
    const G4VTouchable* touchable = step->GetPreStepPoint()->GetTouchable();

    // Retrieve the logical volume of the current step
  
    G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    G4int parentID = step->GetTrack()->GetParentID();
    G4int stepNumber = step->GetTrack()->GetCurrentStepNumber();
  
    G4ParticleDefinition* particle = track->GetDefinition();
    G4double ehitTarget = 0.;
    G4double edepStep = 0.;

    // Define a data structure to store the IDs of particles counted within each logical volume and event
    std::map<G4int, std::map<G4int, std::vector<G4int>>> countedParticles;

    G4String particleName = particle->GetParticleName();
    for (G4int i = 0; i < 100; ++i)
    {
        if (volume == fScoringVolume[i])
        {
            if (volume == fScoringVolume[0])
            {
                if (particleName == "proton")
                {
                    ehitTarget = preStepPoint->GetKineticEnergy();
                    fEventAction->AddEdepTarget(ehitTarget);
                }
            //}
            
            const auto detConstruction = static_cast<const MyDetectorConstruction*>(
            G4RunManager::GetRunManager()->GetUserDetectorConstruction());
            G4double mass = detConstruction->GetScoringVolume(i)->GetMass();

            G4double edepStep = step->GetTotalEnergyDeposit();
            G4double stepSize = step->GetStepLength();
            G4int trackID = track->GetTrackID();
            G4double dosedep = edepStep/mass;
    
            fEventAction->AddEdep(i, edepStep, stepSize, dosedep);
            
            }
            else
            {
                const auto detConstruction = static_cast<const MyDetectorConstruction*>(
                G4RunManager::GetRunManager()->GetUserDetectorConstruction());
                G4double mass = detConstruction->GetScoringVolume(i)->GetMass();

                G4double edepStep = step->GetTotalEnergyDeposit();
                G4double stepSize = step->GetStepLength();
                G4int trackID = track->GetTrackID();
                G4double dosedep = edepStep/mass;
    
                fEventAction->AddEdep(i, edepStep, stepSize, dosedep);
            }
            break;
        }
    }          
}
