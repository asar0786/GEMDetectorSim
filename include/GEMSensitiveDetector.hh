#ifndef GEMSensitiveDetector_h
#define GEMSensitiveDetector_h 1

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class GEMSensitiveDetector : public G4VSensitiveDetector {
public:
    GEMSensitiveDetector(const G4String& name, G4int planeID);
    virtual ~GEMSensitiveDetector() = default;

    virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;

private:
    G4int fPlaneID;
};

#endif
