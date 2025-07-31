#include "GEMDetector.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "GEMSensitiveDetector.hh"
#include "Materials.hh"
#include "HoneycombBuilder.hh"
#include "G4SystemOfUnits.hh"
#include "TString.h"
#include "G4Transform3D.hh"
#include "G4VisAttributes.hh"





GEMDetector::GEMDetector(G4int moduleID, G4Material* gasMat, bool withHoneycomb)
: fModuleID(moduleID), fGasMat(gasMat), fWithHoneycomb(withHoneycomb)
{
    if (fWithHoneycomb)
        ConstructWithHoneycomb();
    else
        ConstructWithoutHoneycomb();
    //Construct();
}

GEMDetector::~GEMDetector() {}

void GEMDetector::Place(const G4Transform3D& transform, G4LogicalVolume* motherLV) {
    new G4PVPlacement(transform, fLogicalVolume,
                      Form("GEMMod%d", fModuleID),
                      motherLV, false, fModuleID, true);
}

    //--------------------Material definations---------------------   
    G4NistManager* nist = G4NistManager::Instance();

    auto vacuum  = nist->FindOrBuildMaterial("G4_Galactic");
    auto FR4     = nist->FindOrBuildMaterial("G4_GLASS_PLATE");
    auto Copper  = nist->FindOrBuildMaterial("G4_Cu");
    auto Kapton  = nist->FindOrBuildMaterial("G4_KAPTON");
    auto silver  = nist->FindOrBuildMaterial("G4_Al");
//    auto silver  = nist->FindOrBuildMaterial("G4_Galactic");
//-----------User defined Materials------------------------
    Materials* matManager = new Materials();
    G4Material* g10Mat = matManager->GetMaterial("G10");
//------------Size parameters----------------------------------
    G4double activeXY = 10*cm;
    G4double outerXY  = 12*cm;
    G4double fr4_drift = 3*mm;
    G4double fr4_readout = 3*mm;
    G4double cu_thick = 5*um;
    G4double kapton_thick = 50*um;
    G4double spacer_thick_drift = 3*mm;
    G4double spacer_thick = 2*mm;
    G4double gasVol3mm = 2.900*mm;
    G4double gasVol2mm = 1.900*mm;
    G4double silverFilm = 25*um;
    G4double g10_thick = 100*um;


void GEMDetector::ConstructWithHoneycomb() {
//--------------------------------------------------------------
    G4double z0 = -0.5 * (fr4_drift + spacer_thick_drift + 3*(kapton_thick + spacer_thick) + fr4_readout + 2*(silverFilm + g10_thick));

    // Module container
    G4double moduleZ = fr4_drift + spacer_thick_drift + 3*(kapton_thick + spacer_thick) + fr4_readout + 2*(silverFilm + g10_thick) + 1*mm;
    auto modBox = new G4Box("GEMModuleBox", outerXY/2 + 1*cm, outerXY/2 + 1*cm, moduleZ/2);
    auto modLogic = new G4LogicalVolume(modBox, vacuum, "GEMModuleLogical");

    // Visualizing (Colors)
    auto visDrift   = new G4VisAttributes(G4Colour(0.2, 0.6, 0.8)); visDrift->SetForceSolid(true);
    auto visCu      = new G4VisAttributes(G4Colour(0.8, 0.5, 0.2)); visCu->SetForceSolid(true);
    auto visKapton  = new G4VisAttributes(G4Colour(1.0, 1.0, 0.3)); visKapton->SetForceSolid(true);
    auto visSpacer  = new G4VisAttributes(G4Colour(0.7, 0.5, 0.2)); visSpacer->SetForceSolid(true);
    auto visSilver  = new G4VisAttributes(G4Colour(0.75, 0.75, 0.75)); visSilver->SetForceSolid(true);
    auto visHoney  = new G4VisAttributes(G4Colour(1.0, 0.01, 0.01)); visHoney->SetForceSolid(true);
    auto visG10  = new G4VisAttributes(G4Colour(0.6, 1.0, 0.6)); visG10->SetForceSolid(true);



    // Aluminium shileding
    auto silverFLM = new G4Box("SilverFLM", outerXY/2, outerXY/2, silverFilm/2);
    auto logicSilverFLM = new G4LogicalVolume(silverFLM, silver, "SilverFLM");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + silverFilm/2), logicSilverFLM, "SilverFLM", modLogic, false, 0);
    logicSilverFLM->SetVisAttributes(visSilver);
    z0 += silverFilm;
    //-------------G10_sheet---------
    auto g10 = new G4Box("G10", outerXY/2, outerXY/2, g10_thick/2);
    auto logicg10 = new G4LogicalVolume(g10, g10Mat, "G10");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + g10_thick/2), logicg10, "G10", modLogic, false, 0);
    z0 += g10_thick;
    logicg10->SetVisAttributes(visG10);
    //--------------------------
    // Drift FR4
    auto outerBox = new G4Box("DriftOuter", outerXY/2, outerXY/2, fr4_drift/2);
    auto innerBox = new G4Box("DriftOuter", activeXY/2, activeXY/2, fr4_drift/2 + 0.1*mm);
    auto driftFR4 = new G4SubtractionSolid("DriftFR4Hole", outerBox, innerBox);
    auto logicDriftFR4 = new G4LogicalVolume(driftFR4, FR4, "DriftFR4Hole");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + fr4_drift/2), logicDriftFR4, "DriftFR4", modLogic, false, 0);
    logicDriftFR4->SetVisAttributes(visDrift);
    // Place honeycomb sheet in the hole, same Z
    G4Box* honeyComb = nullptr;
    G4LogicalVolume* logicHoneyComb = nullptr;
    honeyComb = new G4Box("HoneyComb", activeXY/2, activeXY/2, fr4_drift/2);
    logicHoneyComb = new G4LogicalVolume(honeyComb, vacuum, "HoneyComb");
    HoneycombBuilder::Build(logicHoneyComb, activeXY/2-0.1*cm, activeXY/2-0.3*cm, G4ThreeVector(0, 0, 0));
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + fr4_drift/2), logicHoneyComb, "HoneyComb", modLogic, false, 0, true);
    logicHoneyComb->SetVisAttributes(visHoney);
    z0 += fr4_drift;
//-------Another G10 sheet--------------
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + g10_thick/2), logicg10, "G10", modLogic, false, 0);
    z0 += g10_thick;
    // Drift Cu
    auto driftCu = new G4Box("DriftCu", activeXY/2, activeXY/2, cu_thick/2);
    auto logicDriftCu = new G4LogicalVolume(driftCu, Copper, "DriftCu");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + cu_thick/2), logicDriftCu, "DriftCu", modLogic, false, 0);
    logicDriftCu->SetVisAttributes(visCu);

    // Space gas (2mm)
    auto spaceGas = new G4Box("SpaceGas", activeXY/2, activeXY/2, gasVol2mm/2);
    auto logicSpaceGas = new G4LogicalVolume(spaceGas, fGasMat, "SpaceGas");
    // Drift gas (3mm)

    auto driftGas = new G4Box("DriftGas", activeXY/2, activeXY/2, gasVol3mm/2);
    auto logicDriftGas = new G4LogicalVolume(driftGas, fGasMat, "DriftGas");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + cu_thick + gasVol3mm/2), logicDriftGas, "DriftGas", modLogic, false, 0);


    for (int i = 0; i < 3; ++i){
        G4double thisSpacer = (i == 0) ? spacer_thick_drift : spacer_thick;

        auto outerBox = new G4Box("Outer", outerXY/2, outerXY/2, thisSpacer/2);
        auto innerBox = new G4Box("Inner", activeXY/2, activeXY/2, thisSpacer/2 + 0.1*mm);
        auto spacerSolid = new G4SubtractionSolid("SpacerFrame", outerBox, innerBox);
        auto logicSpacer = new G4LogicalVolume(spacerSolid, FR4, "Spacer");
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + thisSpacer/2), logicSpacer, "Spacer", modLogic, false, i);
        logicSpacer->SetVisAttributes(visSpacer);

        z0 += thisSpacer;

        // Cu1
        auto cuLayer = new G4Box("GEM_Cu", activeXY/2, activeXY/2, cu_thick/2);
        auto logicCu1 = new G4LogicalVolume(cuLayer, Copper, "GEM_Cu1");
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 - cu_thick/2), logicCu1, "GEM_Cu1", modLogic, false, i*3+1);
        logicCu1->SetVisAttributes(visCu);

        // Kapton
        auto kaptonLayer = new G4Box("GEM_Kapton", outerXY/2, outerXY/2, kapton_thick/2);
        auto logicKapton = new G4LogicalVolume(kaptonLayer, Kapton, "GEM_Kapton");
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + kapton_thick/2), logicKapton, "GEM_Kapton", modLogic, false, i*3+2);
        logicKapton->SetVisAttributes(visKapton);

        z0 += kapton_thick;

        // Cu2
        auto logicCu2 = new G4LogicalVolume(cuLayer, Copper, "GEM_Cu2");
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + cu_thick/2), logicCu2, "GEM_Cu2", modLogic, false, i*3+3);
        logicCu2->SetVisAttributes(visCu);

        // Gas gap (2mm)
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + kapton_thick + gasVol2mm/2), logicSpaceGas, "SpaceGas", modLogic, false, i, true);
        std::cout<<"Gas Volume z0+kapton+gasvol2mm:"<<(z0 + kapton_thick + gasVol2mm)<<std::endl;

        if (i == 2){
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + thisSpacer/2), logicSpacer, "Spacer", modLogic, false, i);
        logicSpacer->SetVisAttributes(visSpacer);
        z0 += thisSpacer;
        }
    }

    std::cout<<"Z0 after inductio spacer: "<<z0<<std::endl;

    // Readout Cu
    auto roCu = new G4Box("ReadoutCu", activeXY/2, activeXY/2, cu_thick/2);
    auto logicRO_Cu = new G4LogicalVolume(roCu, Copper, "ReadoutCu");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 - cu_thick/2), logicRO_Cu, "ReadoutCu", modLogic, false, 0, true);
    std::cout<<"Z0 of RO copper: "<<z0-cu_thick/2<<std::endl;
    logicRO_Cu->SetVisAttributes(visCu);

    // Add SD to readout
    auto sdManager = G4SDManager::GetSDMpointer();
    auto sd = new GEMSensitiveDetector("RO_Plane", fModuleID);  // unique ID per replica
    sdManager->AddNewDetector(sd);
    logicRO_Cu->SetSensitiveDetector(sd);
    //-------------G10_sheet---------
    auto g10RO = new G4Box("G10RO", outerXY/2+1*cm, outerXY/2+1*cm, g10_thick/2);
    auto logicg10RO = new G4LogicalVolume(g10RO, g10Mat, "G10RO");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + g10_thick/2), logicg10, "G10RO", modLogic, false, 0);
    z0 += g10_thick;
    logicg10RO->SetVisAttributes(visG10);
    //---------------------------Sample to be deleted later
    auto outerBoxRO = new G4Box("ROOuter", outerXY/2+1*cm, outerXY/2+1*cm, fr4_readout/2);
    auto innerBoxRO = new G4Box("ROInner", activeXY/2, activeXY/2, fr4_readout/2 + 0.1*mm);
    auto readoutSolid = new G4SubtractionSolid("ReadoutSolid", outerBoxRO, innerBoxRO);
    auto logicROSolid = new G4LogicalVolume(readoutSolid, g10Mat, "ReadoutSolid");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + fr4_readout/2), logicROSolid, "ReadoutSolid", modLogic, false, 0);
    logicROSolid->SetVisAttributes(visDrift);
    if (fWithHoneycomb){
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + fr4_readout/2), logicHoneyComb, "HoneyComb", modLogic, false, 0, true);
    }
   z0 += fr4_readout;
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + g10_thick/2), logicg10RO, "G10RO", modLogic, false, 0);
    z0 += g10_thick;
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + silverFilm/2), logicSilverFLM, "SilverFLM", modLogic, false, 0);
    fLogicalVolume = modLogic;

}


void GEMDetector::ConstructWithoutHoneycomb() {
//--------------------------------------------------------------
    G4double z0 = -0.5 * (fr4_drift + spacer_thick_drift + 3*(kapton_thick + spacer_thick) + fr4_readout + 2*(silverFilm));

    // Visualizing (Colors)
    auto visDrift   = new G4VisAttributes(G4Colour(0.2, 0.6, 0.8)); visDrift->SetForceSolid(true);
    auto visCu      = new G4VisAttributes(G4Colour(0.8, 0.5, 0.2)); visCu->SetForceSolid(true);
    auto visKapton  = new G4VisAttributes(G4Colour(1.0, 1.0, 0.3)); visKapton->SetForceSolid(true);
    auto visSpacer  = new G4VisAttributes(G4Colour(0.7, 0.5, 0.2)); visSpacer->SetForceSolid(true);
    auto visSilver  = new G4VisAttributes(G4Colour(0.75, 0.75, 0.75)); visSilver->SetForceSolid(true);
    auto visHoney  = new G4VisAttributes(G4Colour(1.0, 0.01, 0.01)); visHoney->SetForceSolid(true);
    auto visG10  = new G4VisAttributes(G4Colour(0.6, 1.0, 0.6)); visG10->SetForceSolid(true);
    // Module container
    G4double moduleZ = fr4_drift + spacer_thick_drift + 3*(kapton_thick + spacer_thick) + fr4_readout + 2*(silverFilm) + 1*mm;
    auto modBox = new G4Box("GEMModuleBox", outerXY/2 + 1*cm, outerXY/2 + 1*cm, moduleZ/2);
    auto modLogic = new G4LogicalVolume(modBox, vacuum, "GEMModuleLogical");

    // Aluminium shileding
    auto silverFLM = new G4Box("SilverFLM", outerXY/2, outerXY/2, silverFilm/2);
    auto logicSilverFLM = new G4LogicalVolume(silverFLM, silver, "SilverFLM");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + silverFilm/2), logicSilverFLM, "SilverFLM", modLogic, false, 0);
    logicSilverFLM->SetVisAttributes(visSilver);
    z0 += silverFilm;
    // Drift FR4
    auto driftFR4 = new G4Box("DriftFR4", outerXY/2, outerXY/2, fr4_drift/2);
    auto logicDriftFR4 = new G4LogicalVolume(driftFR4, FR4, "DriftFR4");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + fr4_drift/2), logicDriftFR4, "DriftFR4", modLogic, false, 0);
    logicDriftFR4->SetVisAttributes(visDrift);
    z0 += fr4_drift;

    // Drift Cu
    auto driftCu = new G4Box("DriftCu", activeXY/2, activeXY/2, cu_thick/2);
    auto logicDriftCu = new G4LogicalVolume(driftCu, Copper, "DriftCu");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + cu_thick/2), logicDriftCu, "DriftCu", modLogic, false, 0);
    logicDriftCu->SetVisAttributes(visCu);

    // Space gas (2mm)
    auto spaceGas = new G4Box("SpaceGas", activeXY/2, activeXY/2, gasVol2mm/2);
    auto logicSpaceGas = new G4LogicalVolume(spaceGas, fGasMat, "SpaceGas");
    // Drift gas (3mm)

    auto driftGas = new G4Box("DriftGas", activeXY/2, activeXY/2, gasVol3mm/2);
    auto logicDriftGas = new G4LogicalVolume(driftGas, fGasMat, "DriftGas");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + cu_thick + gasVol3mm/2), logicDriftGas, "DriftGas", modLogic, false, 0);

    for (int i = 0; i < 3; ++i){
        G4double thisSpacer = (i == 0) ? spacer_thick_drift : spacer_thick;

        auto outerBox = new G4Box("Outer", outerXY/2, outerXY/2, thisSpacer/2);
        auto innerBox = new G4Box("Inner", activeXY/2, activeXY/2, thisSpacer/2 + 0.1*mm);
        auto spacerSolid = new G4SubtractionSolid("SpacerFrame", outerBox, innerBox);
        auto logicSpacer = new G4LogicalVolume(spacerSolid, FR4, "Spacer");
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + thisSpacer/2), logicSpacer, "Spacer", modLogic, false, i);
        logicSpacer->SetVisAttributes(visSpacer);

        z0 += thisSpacer;

        // Cu1
        auto cuLayer = new G4Box("GEM_Cu", activeXY/2, activeXY/2, cu_thick/2);
        auto logicCu1 = new G4LogicalVolume(cuLayer, Copper, "GEM_Cu1");
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 - cu_thick/2), logicCu1, "GEM_Cu1", modLogic, false, i*3+1);
        logicCu1->SetVisAttributes(visCu);

        // Kapton
        auto kaptonLayer = new G4Box("GEM_Kapton", outerXY/2, outerXY/2, kapton_thick/2);
        auto logicKapton = new G4LogicalVolume(kaptonLayer, Kapton, "GEM_Kapton");
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + kapton_thick/2), logicKapton, "GEM_Kapton", modLogic, false, i*3+2);
        logicKapton->SetVisAttributes(visKapton);

        z0 += kapton_thick;

        // Cu2
        auto logicCu2 = new G4LogicalVolume(cuLayer, Copper, "GEM_Cu2");
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + cu_thick/2), logicCu2, "GEM_Cu2", modLogic, false, i*3+3);
        logicCu2->SetVisAttributes(visCu);


        // Gas gap (2mm)
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + kapton_thick + gasVol2mm/2), logicSpaceGas, "SpaceGas", modLogic, false, i, true);
        std::cout<<"Gas Volume z0+kapton+gasvol2mm:"<<(z0 + kapton_thick + gasVol2mm)<<std::endl;
       // z0 += gasVol2mm;
        if (i == 2){
        new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + thisSpacer/2), logicSpacer, "Spacer", modLogic, false, i);
        logicSpacer->SetVisAttributes(visSpacer);
        z0 += thisSpacer;
        }
    }

    std::cout<<"Z0 after inductio spacer: "<<z0<<std::endl;

    // Readout Cu
    auto roCu = new G4Box("ReadoutCu", activeXY/2, activeXY/2, cu_thick/2);
    auto logicRO_Cu = new G4LogicalVolume(roCu, Copper, "ReadoutCu");
    //new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + cu_thick/2+3.1*mm), logicRO_Cu, "ReadoutCu", modLogic, false, 0);
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 - cu_thick/2), logicRO_Cu, "ReadoutCu", modLogic, false, 0, true);
    std::cout<<"Z0 of RO copper: "<<z0-cu_thick/2<<std::endl;
    logicRO_Cu->SetVisAttributes(visCu);

    // Add SD to readout
    auto sdManager = G4SDManager::GetSDMpointer();
    auto sd = new GEMSensitiveDetector("RO_Plane", fModuleID);  // unique ID per replica
    sdManager->AddNewDetector(sd);
    logicRO_Cu->SetSensitiveDetector(sd);
    // RO FR4 PCB-------
    auto readOutFR4 = new G4Box("ReadOutFR4", outerXY/2+1*cm, outerXY/2+1*cm, fr4_readout/2);
    auto logicReadOutFR4 = new G4LogicalVolume(readOutFR4, FR4, "ReadOutFR4");
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + fr4_readout/2), logicReadOutFR4, "ReadOutFR4", modLogic, false, 0);
    logicReadOutFR4->SetVisAttributes(visDrift);
   z0 += fr4_readout;
    new G4PVPlacement(0, G4ThreeVector(0, 0, z0 + silverFilm/2), logicSilverFLM, "SilverFLM", modLogic, false, 0);
    fLogicalVolume = modLogic;

}


// Factory: Create GEM with honeycomb
GEMDetector* GEMDetector::CreateWithHoneycomb(G4int moduleID, G4Material* gasMat) {
    return new GEMDetector(moduleID, gasMat, true);
}

// Factory: Create GEM without honeycomb
GEMDetector* GEMDetector::CreateWithoutHoneycomb(G4int moduleID, G4Material* gasMat) {
    return new GEMDetector(moduleID, gasMat, false);
}

