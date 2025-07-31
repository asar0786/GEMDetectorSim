#ifndef MATERIALS_HH
#define MATERIALS_HH

#include "G4Material.hh"
#include "G4NistManager.hh"
#include <map>

class Materials {
public:
  Materials();
  ~Materials() = default;

  G4Material* GetMaterial(const G4String& name);

private:
  void DefineMaterials();
  std::map<G4String, G4Material*> fMaterialMap;
};

#endif

