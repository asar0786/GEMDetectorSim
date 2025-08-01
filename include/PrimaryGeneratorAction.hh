#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);
    void SetParticleType(const G4String&);
    void SetEnergy(G4double);

private:
    G4ParticleGun* fParticleGun;
    G4String fParticleType;
    G4double fEnergy;
};

#endif

