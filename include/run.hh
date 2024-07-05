#ifndef RUN_HH
#define RUN_HH

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Accumulable.hh"
#include "G4AccumulableManager.hh"
#include "G4AnalysisManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "generator.hh"
#include "construction.hh"
#include "globals.hh"
#include "G4EventManager.hh"
//#include "g4root.hh"

class MyRunAction : public G4UserRunAction
{
public:
    MyRunAction();
    ~MyRunAction();
    
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    void AddEdep (G4int index, G4double edep, G4double stepLength, G4double AbDose);
    
    void AddEdepTarget (G4double ehitTarget);
    void CountEvent()  { fGoodEvents += 1; };

private:
    
    //G4int numberOfLayers = 100;
    
    G4Accumulable<G4double> fAccumulableDose = 0.;
    G4Accumulable<G4double> fEhitTarget = 0.;
    G4Accumulable<G4int> fGoodEvents = 0;

    G4double fEdep[100];
    G4double fEdep2[100];
    G4double fStep[100];
    G4double fDose[100];

    G4double fLETt[100];
    G4double fLETd[100];

    G4double ftotalLETt[100];
    G4double ftotalLETd[100];
};

#endif
