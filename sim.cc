#include <iostream>

#include "construction.hh"
#include "physics.hh"
#include "action.hh"

#include "G4MTRunManager.hh"
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "QGSP_BIC_HP.hh"
#include "QGSP_BERT_HP.hh"
#include "FTFP_BERT.hh"
#include "G4ScoringManager.hh"



int main(int argc, char** argv)
{

	G4RunManager* runManager = new G4RunManager();
	G4ScoringManager* scManager = G4ScoringManager::GetScoringManager();
	scManager->SetVerboseLevel(1);

	runManager->SetUserInitialization(new MyDetectorConstruction());

//	G4VModularPhysicsList* physicsList = new FTFP_BERT;
	G4VModularPhysicsList* physicsList = new QGSP_BIC_HP;
	physicsList->SetVerboseLevel(1);
	runManager->SetUserInitialization(physicsList);

//	runManager->SetUserInitialization(new PhysicsLists());
	runManager->SetUserInitialization(new MyActionInitialization());

	runManager->Initialize();

	G4UIExecutive* ui = 0;
	
	if (argc == 1)
	{
		ui = new G4UIExecutive(argc, argv);
	}
	G4VisManager* vismanager = new G4VisExecutive();
	vismanager->Initialize();
	G4UImanager* UIManager = G4UImanager::GetUIpointer();

	if (ui)
	{
		//UIManager->ApplyCommand("/control/execute viscore.mac");
		UIManager->ApplyCommand("/control/execute vis.mac");
		ui->SessionStart();
	}
	else
	{
		G4String command = ("/control/execute ");
		G4String filename = argv[1];
		UIManager->ApplyCommand(command + filename);
	}


	delete vismanager;
	delete runManager;
}



