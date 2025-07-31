#include "PrimaryGeneratorAction.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4RandomDirection.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
 : fParticleGun(new G4ParticleGun(1)),
   fParticleType("e-"), fEnergy(6.0*GeV)
{
    SetParticleType(fParticleType);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
}

void PrimaryGeneratorAction::SetParticleType(const G4String& type) {
    fParticleType = type;
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(fParticleType);
    if (particle) {
        fParticleGun->SetParticleDefinition(particle);
    }
}

void PrimaryGeneratorAction::SetEnergy(G4double energy) {
    fEnergy = energy;
    fParticleGun->SetParticleEnergy(fEnergy);
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
     // Uniform random position in 13x13 cm square in XY plane
    //G4double size = 12.0 * cm;   // For parellel beam of slighly larger then detector size
    G4double size = 1.0 * mm;
    G4double x = (G4UniformRand() - 0.5) * size;
    G4double y = (G4UniformRand() - 0.5) * size;
    G4double z = -90.0 * cm; // upstream of detector

    //fParticleGun->SetParticlePosition(G4ThreeVector(x-6.3*cm, y, z));
    // Define rotation: 18° in XZ plane → rotate around Y-axis
    //G4double theta = 18.0 * deg;
    //G4ThreeVector direction(std::sin(theta), 0.0, std::cos(theta)); // unit vector
    //fParticleGun->SetParticleMomentumDirection(G4ThreeVector(direction));

    fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
    fParticleGun->SetParticleEnergy(fEnergy); // already set via CLI or default
    fParticleGun->GeneratePrimaryVertex(event);
}

