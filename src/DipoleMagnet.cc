#include "DipoleMagnet.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"

DipoleMagnet::DipoleMagnet(G4double fieldStrength, G4double dx, G4double dy, G4double dz)
  : fFieldStrength(fieldStrength), fSizeX(dx), fSizeY(dy), fSizeZ(dz)
{}

G4VPhysicalVolume* DipoleMagnet::Construct(G4LogicalVolume* motherLV)
{
    // Create magnetic field and field manager
    G4ThreeVector fieldVector(0.0, fFieldStrength, 0.0);  // Field along Y
    auto uniformField = new G4UniformMagField(fieldVector);
    auto fieldManager = new G4FieldManager(uniformField);

    // Create dipole box
    auto magnetBox = new G4Box("DipoleBox", fSizeX/2 + 1*mm, fSizeY/2 + 1*mm, fSizeZ/2 + 1*mm);
    auto nist = G4NistManager::Instance();
    auto air = nist->FindOrBuildMaterial("G4_AIR");
    auto logicDipole = new G4LogicalVolume(magnetBox, air, "DipoleBoxLV");

    // Set field manager to this logical volume
    logicDipole->SetFieldManager(fieldManager, true);

    // Add visual attributes
    auto vis = new G4VisAttributes(G4Colour(0.8, 0.8, 0.0)); // light yellow
    vis->SetForceSolid(true);
    logicDipole->SetVisAttributes(vis);

    // Place the magnet at origin inside the mother volume
    auto physMagnet = new G4PVPlacement(
        nullptr,                      // no rotation (already applied in mother placement)
        G4ThreeVector(0, 0, 0),       // place at center of mother
        logicDipole,
        "DipoleBoxPV",
        motherLV,
        false,
        0,
        true
    );

    return physMagnet;
}

