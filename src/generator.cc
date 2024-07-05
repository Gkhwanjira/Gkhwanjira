#include "generator.hh"

MyPrimaryGenerator::MyPrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1);
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName="proton";
    G4ParticleDefinition *particle = particleTable->FindParticle("proton");

    G4ThreeVector pos(0.,0.,0.);
    G4ThreeVector mom(0.,0.,-1.);

    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleMomentumDirection(mom);
    //fParticleGun->SetParticleMomentum(100.*MeV);
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(70.*MeV);
    //fParticleGun->GeneratePrimaryVertex(anEvent);

    G4double envSize = 0.;

    if (!fTube)
    {
        G4LogicalVolume *pipe = G4LogicalVolumeStore::GetInstance()->GetVolume("TubeLogical");
        if (pipe) fTube = dynamic_cast<G4Tubs*>(pipe->GetSolid());
       
    }

    if (fTube)
    {
        envSize = fTube->GetOuterRadius()/ 2.5; 
    }
    
    else  
    {
        G4ExceptionDescription msg;
        msg << "Envelope volume of tube shape not found.\n";
        msg << "Perhaps you have changed geometry.\n";
        msg << "The gun will be place at the center.";
        G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
        "MyCode0002",JustWarning,msg);
    }
    
    G4double randomRadius = G4RandFlat::shoot(0.0, envSize);
    G4double randomTheta = G4RandFlat::shoot(0.0, 360*deg);
  
    G4double x0 = randomRadius * std::cos(randomTheta);
    G4double y0 = randomRadius * std::sin(randomTheta);
    G4double z0 = 100.*cm; 

    fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
      fParticleGun->GeneratePrimaryVertex(anEvent);
}
