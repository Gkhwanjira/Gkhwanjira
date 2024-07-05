#include "detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
    G4Track *track = aStep->GetTrack();

    //track->SetTrackStatus(fStopAndKill);

    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint *postStepPoint = aStep->GetPostStepPoint();

    G4ParticleDefinition* particle = track->GetDefinition();
    G4String particleName = particle->GetParticleName();
    G4int parentID = track->GetParentID();
    
    if (particleName == "proton" && parentID == 0)
    {
        G4ThreeVector posProton = preStepPoint->GetPosition();
        G4ThreeVector momProton = preStepPoint->GetMomentum();

        G4double time = preStepPoint->GetGlobalTime();
        G4double Edep = preStepPoint->GetKineticEnergy();

        //G4cout << "Proton position: " << posProton << G4endl;

        const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();


        //G4cout << "Copy number: " << copyNo << G4endl;

        G4VPhysicalVolume *physVol = touchable->GetVolume();
        G4ThreeVector posDetector = physVol->GetTranslation();

        /*const auto detConstruction = static_cast<const MyDetectorConstruction*>(
        G4RunManager::GetRunManager()->GetUserDetectorConstruction());
        G4double mass = detConstruction->GetScoringVolume(0)->GetMass();

        G4double dose1 = Edep / mass;*/
        
        #ifndef G4MULTITHREADED
        //G4cout << "Detector position: " << posDetector << G4endl;
        //G4cout << "Proton wavelength: " << wlen << G4endl;
        #endif

        G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

        G4AnalysisManager *man = G4AnalysisManager::Instance();

        man->FillNtupleIColumn(1, 0, evt);
        man->FillNtupleDColumn(1, 1, posProton[0]);
        man->FillNtupleDColumn(1, 2, posProton[1]);
        man->FillNtupleDColumn(1, 3, posProton[2]);
        man->FillNtupleDColumn(1, 4, time);
        man->FillNtupleDColumn(1, 5, Edep);
        //man->FillNtupleDColumn(1, 6, dose1);
        man->AddNtupleRow(1);

    }

    return true;
}
