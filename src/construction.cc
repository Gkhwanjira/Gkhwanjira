#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{
    xWorld = 2.0*m;
    yWorld = 2.0*m;
    zWorld = 2.0*m;

    G4NistManager *nist = G4NistManager::Instance();
    worldMat = nist->FindOrBuildMaterial("G4_AIR");
    PMMA = nist->FindOrBuildMaterial("G4_PLEXIGLASS");
    vacuum = new G4Material("Vacuum", 1.0, 1.01*g/mole, 6.66612e-5*pascal, kStateGas, 293*kelvin);
    water = nist->FindOrBuildMaterial("G4_WATER");
    Al = nist->FindOrBuildMaterial("G4_Al");

    //world//
    solidWorld = new G4Box("solidWorld", xWorld, yWorld, zWorld);
    logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
    physWorld = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), logicWorld, "physWorld", 0, false, 0, true);

    //extraction layer1 
    solidTube = new G4Tubs("Tube", 0., 2.5*cm, 3.*cm/2, 0., 360.0*deg);
    logicTube = new G4LogicalVolume(solidTube, vacuum, "TubeLogical");
    physTube = new G4PVPlacement(0,G4ThreeVector(0., 0., 101.5*cm),logicTube,"TubePhysical", logicWorld,false, 0,fCheckOverlaps);
    
    //Plane 
    solidPlane = new G4Box("Plane", 10*cm, 10*cm, .05*mm);
    logicPlane = new G4LogicalVolume(solidPlane, vacuum, "PlaneLV");
    physPlane = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0.05*mm),                           
                                      logicPlane, "PlanePV", logicWorld, false, 0, fCheckOverlaps);


    //Phantom 
    boxSizeX = 180.0 * mm;
    boxSizeY = 240.0 * mm;
    boxSizeZ = 210.0 * mm;
    PMMAThickness = 10.0 * mm;

    solidBoxOuter = new G4Box("BoxOuter", boxSizeX / 2, boxSizeY / 2, boxSizeZ / 2);
    solidBoxInner = new G4Box("BoxInner", (boxSizeX - 2 * PMMAThickness) / 2, (boxSizeY - 2 * PMMAThickness) / 2, boxSizeZ / 2);

    G4SubtractionSolid*solidBox = new G4SubtractionSolid("PMMABox", solidBoxOuter, solidBoxInner);
    logicBox = new G4LogicalVolume(solidBox, PMMA, "PMMABox");
    physBox = new G4PVPlacement(0, G4ThreeVector(), logicBox, "PMMABox", logicWorld, false, 0, fCheckOverlaps);

    solidBBox = new G4Box("BackPMMABox", (boxSizeX / 2) - PMMAThickness, (boxSizeY / 2) - PMMAThickness, PMMAThickness / 2);
    logicBBox = new G4LogicalVolume(solidBBox, PMMA, "BackPMMABox");
    physBBox = new G4PVPlacement(0, G4ThreeVector(0., 0., -100.*mm), logicBBox, "BackPMMABox", logicWorld, false, 0, fCheckOverlaps);

    // Aluminum sheet
    Alu_sizeX = (boxSizeX / 2) - PMMAThickness;
    Alu_sizeY = (boxSizeY / 2) - PMMAThickness;
    Al1_sizeZ = 18. * mm; //define Al1 sheet thickness here
    //Al2_sizeZ = 0. * mm;  //define Al2 sheet thickness here
    //Al2LtoTarget = 10. * cm; //length of Al2 to target

    solidAl1 = new G4Box("Alsheet1", Alu_sizeX, Alu_sizeY, Al1_sizeZ / 2);
    logicAl1 = new G4LogicalVolume(solidAl1, Al, "Alsheet1LV");
    physAl1 = new G4PVPlacement(nullptr,G4ThreeVector(0., 0., 97.5*cm),logicAl1,
                "Alsheet1PV",logicWorld,false,0,fCheckOverlaps);

    /*solidAl2 = new G4Box("Alsheet2", 40*mm, 55*mm, 0.5*mm); 
    logicAl2 = new G4LogicalVolume(solidAl2, Al, "Alsheet2LV");
    physAl2 = new G4PVPlacement(nullptr,G4ThreeVector(0., 0., Al2LtoTarget + Al2_sizeZ / 2), //10.25 for Al foil 5 mm
                logicAl2,"Alsheet2PV",logicWorld,false,0,fCheckOverlaps);*/


    //create target and volume
    G4double target_radius = 17.5*mm;
    G4double target_dZ = 0.1*mm;

    G4int numberOfLayers = 100;

    G4LogicalVolume* logicTarget[100];


    for(G4int i = 0; i < numberOfLayers; ++i){

      G4String logicVolName = "Target_" + std::to_string(i+1);

      // Create the logical volume
      auto solidTarget = new G4Tubs(logicVolName, 0., target_radius, 0.5 * target_dZ, 0., 360*deg);
           logicTarget[i] = new G4LogicalVolume(solidTarget, water, logicVolName);

      new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -0.5*target_dZ - target_dZ*i), logicTarget[i], logicVolName, logicWorld,false, 0,fCheckOverlaps); 

      fScoringVolume[i] = logicTarget[i];
    
     }
    
    return physWorld;
}



void MyDetectorConstruction::ConstructSDandField()
{
    MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");

    if(logicPlane != NULL)
        logicPlane->SetSensitiveDetector(sensDet);
}