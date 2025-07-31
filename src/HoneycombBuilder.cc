#include "HoneycombBuilder.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4ExtrudedSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"

#include <vector>

void HoneycombBuilder::Build(G4LogicalVolume* motherLV, double sizeX, double sizeY, const G4ThreeVector& center) {
    // Parameters
    G4double outerRadius = 3.0 * mm;
    G4double wallThickness = 0.5 * mm;
    G4double thickness = 3.0 * mm;

    // Vertices of regular hexagon
    std::vector<G4TwoVector> hexOuter, hexInner;
    for (int i = 0; i < 6; ++i) {
        double angle = i * 60.0 * deg;
        hexOuter.emplace_back(outerRadius * cos(angle), outerRadius * sin(angle));
        hexInner.emplace_back((outerRadius - wallThickness) * cos(angle), (outerRadius - wallThickness) * sin(angle));
    }

    // Solids
    auto outerSolid = new G4ExtrudedSolid("HexOuter", hexOuter, thickness/2, {}, 1.0, {}, 1.0);
    auto innerSolid = new G4ExtrudedSolid("HexInner", hexInner, thickness/2+0.01*mm, {}, 1.0, {}, 1.0);
    auto wallSolid  = new G4SubtractionSolid("HexWall", outerSolid, innerSolid);

    // Material and logic
    auto mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_POLYETHYLENE");
    auto logicHex = new G4LogicalVolume(wallSolid, mat, "HexWallLV");

    // Visualization
    //auto vis = new G4VisAttributes(G4Colour(0.5, 0.3, 0.1)); // Brownish
    auto vis = new G4VisAttributes(G4Colour(1.0, 0.3, 0.1)); // Dark Brownish
    vis->SetForceSolid(true);
    vis->SetForceAuxEdgeVisible(true); // <--- shows internal edges of boolean solids
    vis->SetLineWidth(2);              // Optional: thickens edges for clarity
    logicHex->SetVisAttributes(vis);

    // Grid placement
    int nx = static_cast<int>(sizeX / (1.5 * outerRadius));
    int ny = static_cast<int>(sizeY / (sqrt(3.0) * outerRadius));

    for (int ix = -nx; ix <= nx; ++ix) {
        for (int iy = -ny; iy <= ny; ++iy) {
            double x = ix * 1.5 * outerRadius;
            double y = iy * sqrt(3.0) * outerRadius;
            if (ix % 2 != 0) y += 0.5 * sqrt(3.0) * outerRadius;

            new G4PVPlacement(
                nullptr,
                center + G4ThreeVector(x, y, 0),
                logicHex,
                "HexCell",
                motherLV,
                false,
                ix * 1000 + iy
            );
        }
    }
}

