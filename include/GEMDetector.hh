#ifndef GEMDETECTOR_HH
#define GEMDETECTOR_HH

#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"


class GEMDetector {
public:
    GEMDetector(G4int moduleID, G4Material* gasMat, bool withHoneycomb = true);
    ~GEMDetector();

    static GEMDetector* CreateWithHoneycomb(G4int moduleID, G4Material* gasMat);
    static GEMDetector* CreateWithoutHoneycomb(G4int moduleID, G4Material* gasMat);


    G4LogicalVolume* GetLogicalVolume() const { return fLogicalVolume; }
    void Place(const G4Transform3D& transform, G4LogicalVolume* motherLV);

private:
    G4LogicalVolume* fLogicalVolume = nullptr;
    G4Material* fGasMat = nullptr;
    G4int fModuleID = 0;
    bool fWithHoneycomb;

    //void Construct();  // Called internally during construction
    void ConstructWithHoneycomb();
    void ConstructWithoutHoneycomb();
};

#endif

