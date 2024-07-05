#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4SubtractionSolid.hh"
#include "detector.hh"
#include <iostream>


class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
    MyDetectorConstruction();
    ~MyDetectorConstruction();

    virtual G4VPhysicalVolume *Construct() override;

    // Getter method to access the scoring volumes
    G4LogicalVolume* GetScoringVolume(G4int index) const
    {
         return fScoringVolume[index];
    }


private:
    G4Box *solidWorld, *solidBoxOuter, *solidBoxInner, *solidBBox, *solidPlane,*solidTarget, 
        *solidAl1,*solidAl2;

    G4Tubs *solidTube;

    G4LogicalVolume *logicWorld, *logicTube, *logicBox, *logicBBox, *logicTarget, 
        *logicAl1, *logicAl2;

    G4VPhysicalVolume *physWorld, *physTube, *physBox, *physBBox, *physPlane, *physTarget, 
        *physAl1, *physAl2;
   
    G4Material *worldMat, *PMMA, *vacuum, *water, *Al;
   
    G4double xWorld, yWorld, zWorld, boxSizeX,boxSizeY, boxSizeZ, PMMAThickness,
            Alu_sizeX, Alu_sizeY, Al1_sizeZ, Al2_sizeZ, Al2LtoTarget;
    
    G4bool fCheckOverlaps = true;

    // Array of logical volumes for scoring
    G4LogicalVolume* fScoringVolume[100];
    G4LogicalVolume* logicPlane;

    virtual void ConstructSDandField();

   
};

#endif
