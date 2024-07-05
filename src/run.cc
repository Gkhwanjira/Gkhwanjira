#include "run.hh"

MyRunAction::MyRunAction()
{
    // add new units for dose
  
    const G4double milligray = 1.e-3*gray;
    const G4double microgray = 1.e-6*gray;
    const G4double nanogray  = 1.e-9*gray;
    const G4double picogray  = 1.e-12*gray;

    new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
    new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
    new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
    new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);
    
    // Register accumulable to the accumulable manager
    for (G4int i = 0; i < 100; ++i)
    {
        fEdep[i] = 0.;
        fEdep2[i] = 0.;
        fStep[i] = 0.;
        fDose[i] = 0.;
    }
    
    fEhitTarget = 0.;
    fGoodEvents = 0;
    fAccumulableDose = 0.;

    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();

    accumulableManager->RegisterAccumulable(fEhitTarget);
    accumulableManager->RegisterAccumulable(fGoodEvents);
    accumulableManager->RegisterAccumulable(fAccumulableDose);

    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->CreateNtuple("Edep_Dose_LETt_LETd", "Edep_Dose_LETt_LETd");
    man->CreateNtupleIColumn("Volume");
    man->CreateNtupleDColumn("Total_Energy_Deposit(MeV)");
    man->CreateNtupleDColumn("Dose(Gy)");
    man->CreateNtupleDColumn("LETt(keV/um)");
    man->CreateNtupleDColumn("LETd(keV/um)");
    man->CreateNtupleDColumn("Mean_EhitTarget(MeV)");
    man->CreateNtupleIColumn("NumberOfGoodEvents");
    man->FinishNtuple(0);

    man->CreateNtuple("Protons", "Protons");
    man->CreateNtupleIColumn("fEvent");
    man->CreateNtupleDColumn("fX");
    man->CreateNtupleDColumn("fY");
    man->CreateNtupleDColumn("fZ");
    man->CreateNtupleDColumn("fT");
    man->CreateNtupleDColumn("fEdep");
    man->CreateNtupleDColumn("fdose1");
    man->FinishNtuple(1);

}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
    // inform the runManager to save random number seed
    G4RunManager::GetRunManager()->SetRandomNumberStore(false);
    
    // reset accumulables to their initial values
    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Reset();

    G4AnalysisManager *man = G4AnalysisManager::Instance();

    G4int runID = run->GetRunID();

    std::stringstream strRunID;
    strRunID << runID;

    man->OpenFile("output"+strRunID.str()+".csv");
}

void MyRunAction::AddEdep(G4int index, G4double edep, G4double stepLength, G4double AbDose)
{
    if (index >= 0 && index < 100)
    {
        fEdep[index] += edep;
        fEdep2[index] += edep*edep;
        fStep[index] += stepLength;
        fDose[index] += AbDose;
        fAccumulableDose += AbDose;
    }
    else
    {
        fEdep[index] += 0.;
        fEdep2[index] += 0.;
        fStep[index] += 0.;
        fDose[index] += 0.;
        fAccumulableDose += 0.;
    }
}

void MyRunAction::AddEdepTarget(G4double ehitTarget)
{
    fEhitTarget += ehitTarget;
}

void MyRunAction::EndOfRunAction(const G4Run* run)
{
    G4int nofEvents = run->GetNumberOfEvent();
    if (nofEvents == 0) return;

    // Merge accumulables
    G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
    accumulableManager->Merge();
    
    // Compute dose = total energy deposit in a run and its variance
  
    const auto detConstruction = static_cast<const MyDetectorConstruction*>(
    G4RunManager::GetRunManager()->GetUserDetectorConstruction());

    G4double dose = 0.;
    G4double totalDose = 0.;
    G4double LETt = 0.;
    G4double LETd = 0.;
    
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    for (G4int i = 0; i < 100; ++i)
    {
        G4double edep  = fEdep[i];
        G4double edep2 = fEdep2[i];
        G4double step  = fStep[i];
                dose = fDose[i];

                LETt = edep/step;
                LETd = edep2/step;
                totalDose += dose;

         /*G4cout << "edep in volume " << i+1 << " = " << edep / MeV << G4endl
         << " with dose " << G4BestUnit(dose, "Dose") << G4endl
         << "with LETt " << LETt / (keV/um) << G4endl;*/

        man->FillNtupleIColumn(0, 0, i+1);
        man->FillNtupleDColumn(0, 1, edep / MeV);
        man->FillNtupleDColumn(0, 2, dose / gray);

        if (edep > 0. && step > 0.)
        {
            man->FillNtupleDColumn(0, 3, LETt / (keV/um));
            man->FillNtupleDColumn(0, 4, LETd / (keV/um));
        }else
        {   man->FillNtupleDColumn(0, 3, 0);
            man->FillNtupleDColumn(0, 4, 0);
        }
        
        if (i == 0)
        {
            man->FillNtupleDColumn(0, 5, (fEhitTarget.GetValue()/fGoodEvents.GetValue()) / MeV);
            man->FillNtupleIColumn(0, 6, fGoodEvents.GetValue());
        }
        man->AddNtupleRow(0); 
    }
    
    man->Write();
    man->CloseFile();

    const auto generatorAction = static_cast<const MyPrimaryGenerator*>(
    G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    G4String runCondition;

    if (generatorAction)
    {
        const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
        runCondition += particleGun->GetParticleDefinition()->GetParticleName();
        runCondition += " of ";
        G4double particleEnergy = particleGun->GetParticleEnergy();
        runCondition += G4BestUnit(particleEnergy,"Energy");
    }
    
    // Print
    //
    if (IsMaster()) {
        G4cout
        << G4endl
        << "--------------------End of Global Run-----------------------";
    }
    else {
        G4cout
        << G4endl
        << "--------------------End of Local Run------------------------";
    }

    G4cout
        << G4endl
        << " Energy hit Target: " << (fEhitTarget.GetValue()/fGoodEvents.GetValue()) / MeV 
        << " MeV" << " and number of particles hitting the target: " << fGoodEvents.GetValue()
        << G4endl
        << " The run consists of " << nofEvents << " "<< runCondition
        << G4endl
        << " Cumulated dose per run, in scoring volume : "
        << G4BestUnit(fAccumulableDose.GetValue(),"Dose")  << G4endl
        << "------------------------------------------------------------"
        << G4endl
        << G4endl;

}
