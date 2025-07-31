#include "GEMDetectorConstruction.hh"
#include "GEMSensitiveDetector.hh"
#include "TString.h"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "HoneycombBuilder.hh"
#include "Materials.hh"
#include "DipoleMagnet.hh"
#include "GEMDetector.hh"

GEMDetectorConstruction::GEMDetectorConstruction() {}
GEMDetectorConstruction::~GEMDetectorConstruction() {}

G4VPhysicalVolume* GEMDetectorConstruction::Construct() {
    G4NistManager* nist = G4NistManager::Instance();
    auto vacuum = nist->FindOrBuildMaterial("G4_Galactic");
    auto worldMat = nist->FindOrBuildMaterial("G4_AIR");
    auto copper = nist->FindOrBuildMaterial("G4_Cu");
    auto Ar = nist->FindOrBuildMaterial("G4_Ar");
    auto CO2 = nist->FindOrBuildMaterial("G4_CARBON_DIOXIDE");
    auto visCu  = new G4VisAttributes(G4Colour(1.0, 0.15, 0.15)); visCu->SetForceSolid(true);
    //auto visTargetLH2  = new G4VisAttributes(G4Colour(0.15, 0.15, 0.15)); visTargetLH2->SetForceSolid(true);
    auto visTargetLH2  = new G4VisAttributes(G4Colour(1.0, 1.0, 0.88)); visTargetLH2->SetForceSolid(true);

    //--------ArCO2 for GEM Gas
    Materials* matManager = new Materials();
    G4Material* ArCO2 = matManager->GetMaterial("ArCO2_7030");
//--------------World defination-----
    G4double worldSize = 600*cm;
    auto solidWorld = new G4Box("World", worldSize/2-100*cm, worldSize/2-100*cm, worldSize/2);
    auto logicWorld = new G4LogicalVolume(solidWorld, worldMat, "World");
    auto physWorld = new G4PVPlacement(0, {}, logicWorld, "World", 0, false, 0);
//    //---------Target------------
    G4Material* LH2 = matManager->GetMaterial("lH2");
    auto targetLH2 = new G4Box("TargetLH2", 1.0*cm, 1.0*cm, 15.0*cm);
    auto logicTargetLH2 = new G4LogicalVolume(targetLH2, LH2, "TargetLH2");
    new G4PVPlacement(0, G4ThreeVector(0, 0, -70*cm), logicTargetLH2, "TargetLH2", logicWorld, false, 0, true);
    //new G4PVPlacement(rotY, G4ThreeVector(0, 0, -70*cm), logicTargetLH2, "TargetLH2", logicWorld, false, 0, true);
    logicTargetLH2->SetVisAttributes(visTargetLH2);


    // Build and place multiple GEM modules
    G4int sdx;
    G4int nModules = 8;
    G4double s0 = 160.0 * cm;
    G4double spacing = 10*cm;
    G4double theta_deg = 18.0;  // Set your andle of rotation of Detectors respective of target as origion
    G4double theta = theta_deg * deg;
    G4double targetZ = - 70 * cm;
    for (int i = 0; i < nModules; ++i) {
        //auto gem = GEMDetector::CreateWithHoneycomb(i, ArCO2);   // Drift and RO are solid FR4 
        auto gem = GEMDetector::CreateWithoutHoneycomb(i, ArCO2);  // Drift and RO is filled with Honeycomb 
        // s = linear distance along the theta_deg inclined axis, starting from the target
        G4double s = s0 + i * spacing;

        // Compute new position rotated from target center
        G4double x = sin(theta) * s;
        G4double y = 0;
        G4double z = targetZ + cos(theta) * s;

    // Rotation: rotate GEM to face beam direction at theta_deg
        G4RotationMatrix* rotY = new G4RotationMatrix();
        rotY->rotateY(theta);

    // Combine into placement
        G4Transform3D transform(*rotY, G4ThreeVector(x, y, z));
        gem->Place(transform, logicWorld);

        sdx=i;
    }
    //Shielding plate for photons generated at target
//    auto shieldingCu = new G4Box("ShieldingCu", 5*cm, 5.0*cm, 0.5*mm/2);
//    auto logicshieldingCu = new G4LogicalVolume(shieldingCu, copper, "ShieldingCu");
//    new G4PVPlacement(rotY, G4ThreeVector(0, 0, -0.4*m), logicshieldingCu, "ShieldingCu", logicWorld, false, 0, true);
//    logicshieldingCu->SetVisAttributes(visCu);
    //Dipole construction...
    // Dipole Magnet parameters
    G4double dipoleDistanceFromTarget = 90.0 * cm; // distance from target center
    G4double dipoleZ = targetZ + dipoleDistanceFromTarget; // absolute Z of dipole center

    // Dipole rotation: theta_deg around Y-axis
    G4RotationMatrix* rotYM = new G4RotationMatrix();
    rotYM->rotateY(-18.0 * deg);

    // Dipole center in rotated coordinate
    G4double dipoleX = std::sin(theta) * dipoleDistanceFromTarget;
    G4double dipoleZRot = std::cos(theta) * dipoleDistanceFromTarget + targetZ;

    // Position vector (Y remains 0)
    G4ThreeVector dipolePos(dipoleX, 0, dipoleZRot);

    // Construct and place dipole magnet
    G4double fieldStrnghth = 1.0; //Set your magnetic field here
    DipoleMagnet* dipole = new DipoleMagnet(fieldStrnghth * tesla, 12 * cm, 12 * cm, 90 * cm); // size as per your build 
    auto magnetBox = new G4Box("MagnetBox", 6.0 * cm, 6.0 * cm, 45.0 * cm);  // half-dimensions
    auto logicMagnet = new G4LogicalVolume(magnetBox, worldMat, "MagnetBoxLV");
    new G4PVPlacement(rotYM, dipolePos, logicMagnet, "DipoleMagnetPV", logicWorld, false, 0, true);
    dipole->Construct(logicMagnet);


    // Scoring plane 1: right above readout
    auto sdManager = G4SDManager::GetSDMpointer();
    auto scoring1 = new G4Box("Scoring1", 5*cm, 5*cm , 0.005*mm);
    auto logicScore1 = new G4LogicalVolume(scoring1, vacuum, "ScoringPlane1");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0.97*m), logicScore1, "ScoringPlane1", logicWorld, false, 0, true);
    // Scoring plane infront of GEMs
    G4double s1 = 159*cm;
    G4double x1 = sin(theta) * s1;
    G4double y1 = 0;
    G4double z1 = cos(theta) * s1 + targetZ;
    auto scoring2 = new G4Box("Scoring2", 5*cm, 5*cm , 0.005*mm);
    auto logicScore2 = new G4LogicalVolume(scoring2, vacuum, "ScoringPlane2");
    new G4PVPlacement(rotYM, G4ThreeVector(x1, y1, z1), logicScore2, "ScoringPlane2", logicWorld, false, 0, true);
    //new G4PVPlacement(0, G4ThreeVector(0, 0, 37.0*cm), logicScore1, "ScoringPlane1", logicWorld, false, 0, true);

    //auto sd1 = new GEMSensitiveDetector("Score1");
    auto sd1 = new GEMSensitiveDetector("Score1", sdx+1);
    auto sd2 = new GEMSensitiveDetector("Score2", sdx+2);
    sdManager->AddNewDetector(sd1);
    sdManager->AddNewDetector(sd2);
    logicScore1->SetSensitiveDetector(sd1);
    logicScore2->SetSensitiveDetector(sd2);

    return physWorld;
}

