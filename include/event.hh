#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"

//#include "g4root.hh"
#include "construction.hh"
#include "run.hh"
#include "G4AnalysisManager.hh"

class MyEventAction : public G4UserEventAction
{
public:
    MyEventAction(MyRunAction*);
    ~MyEventAction();
    
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    void AddEdep(G4int index, G4double edep, G4double stepLength, G4double AbDose)
    {
        fEdep[index] += edep;
        fStep[index] += stepLength;
        fDose[index] += AbDose;
    }
    
    void AddEdepTarget(G4double ehitTarget) { fEhitTarget += ehitTarget; } 
    
private:
    MyRunAction *fRunAction;
    G4double   fEdep[100];
    G4double   fStep[100];
    G4double   fDose[100];
    G4double   fEhitTarget = 0.;
};

#endif
