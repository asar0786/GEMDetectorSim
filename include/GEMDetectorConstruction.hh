#ifndef GEMDetectorConstruction_h
#define GEMDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4VSensitiveDetector.hh"
#include "GEMSensitiveDetector.hh"


class GEMDetectorConstruction : public G4VUserDetectorConstruction {
  public:
    GEMDetectorConstruction();
    virtual ~GEMDetectorConstruction();
    virtual G4VPhysicalVolume* Construct();
    G4LogicalVolume* BuildGEMModule(G4Material* gas, G4int moduleID);

};

#endif

