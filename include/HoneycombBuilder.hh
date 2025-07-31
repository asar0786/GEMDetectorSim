#ifndef HONEYCOMB_BUILDER_HH
#define HONEYCOMB_BUILDER_HH

#include "G4ThreeVector.hh"
class G4LogicalVolume;

class HoneycombBuilder {
public:
    static void Build(G4LogicalVolume* motherLV, double sizeX, double sizeY, const G4ThreeVector& center);
};

#endif

