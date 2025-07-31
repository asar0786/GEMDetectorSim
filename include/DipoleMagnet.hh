#ifndef DipoleMagnet_h
#define DipoleMagnet_h 1

#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "globals.hh"

class DipoleMagnet {
public:
    // Constructor with field strength and size parameters
    DipoleMagnet(G4double fieldStrength, G4double dx, G4double dy, G4double dz);

    // Construct inside mother logical volume
    G4VPhysicalVolume* Construct(G4LogicalVolume* motherLV);

private:
    G4double fFieldStrength;  // Magnetic field strength in Tesla
    G4double fSizeX;          // Width (X)
    G4double fSizeY;          // Height (Y)
    G4double fSizeZ;          // Length (Z)
};

#endif

