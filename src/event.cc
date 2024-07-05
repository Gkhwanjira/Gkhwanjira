#include "event.hh"

MyEventAction::MyEventAction(MyRunAction *runAction)
{
    fRunAction = runAction;
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
    for (G4int i = 0; i < 100; ++i)
    {
        fEdep[i] = 0.;
        fStep[i] = 0.;
        fDose[i] = 0.;
    }
    
    fEhitTarget = 0.;
}

void MyEventAction::EndOfEventAction(const G4Event*)
{
    G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

    for (G4int index = 0; index < 100; ++index)
    {
        fRunAction->AddEdep(index, fEdep[index], fStep[index], fDose[index]);

    }
    
    fRunAction->AddEdepTarget(fEhitTarget);
    if(fEhitTarget > 0) fRunAction->CountEvent();

}
