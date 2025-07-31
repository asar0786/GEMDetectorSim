#include "Materials.hh"
#include "G4SystemOfUnits.hh"
#include "G4Element.hh"

Materials::Materials() {
  DefineMaterials();
}

void Materials::DefineMaterials() {
  auto nist = G4NistManager::Instance();

  // Elements
  G4Element* elH  = nist->FindOrBuildElement("H");
  G4Element* elC  = nist->FindOrBuildElement("C");
  G4Element* elO  = nist->FindOrBuildElement("O");
  G4Element* elSi = nist->FindOrBuildElement("Si");
  G4Element* elAr = nist->FindOrBuildElement("Ar");

  // ---------------- G10 ----------------
  // Epoxy (C18H20O4)
  G4Material* Epoxy = new G4Material("Epoxy", 1.2*g/cm3, 3);
  Epoxy->AddElement(elC, 18);
  Epoxy->AddElement(elH, 20);
  Epoxy->AddElement(elO, 4);

  // Quartz / SiO2
  G4Material* SiO2 = new G4Material("Quartz", 2.2*g/cm3, 2);
  SiO2->AddElement(elSi, 1);
  SiO2->AddElement(elO, 2);

  // G10 ~ 60% SiO2 + 40% Epoxy
  G4Material* G10 = new G4Material("G10", 1.7*g/cm3, 2);
  G10->AddMaterial(SiO2, 60.*perCent);
  G10->AddMaterial(Epoxy, 40.*perCent);
  fMaterialMap["G10"] = G10;

  // ---------------- Ar/CO2 (70:30) ----------------
  G4Material* CO2 = nist->FindOrBuildMaterial("G4_CARBON_DIOXIDE");
  G4Material* Ar  = nist->FindOrBuildMaterial("G4_Ar");

  G4Material* ArCO2_7030 = new G4Material("ArCO2_7030", 1.822*mg/cm3, 2,
          kStateGas, 293.15*kelvin, 1*atmosphere);
  ArCO2_7030->AddMaterial(Ar, 70.*perCent);
  ArCO2_7030->AddMaterial(CO2, 30.*perCent);
  fMaterialMap["ArCO2_7030"] = ArCO2_7030;
  //--------Liquid Hydrogen at 22K ---------
  // ---------------- Liquid Hydrogen at 22K ----------------
  G4Material* LH2 = new G4Material("LH2", 0.0708*g/cm3, 1, kStateLiquid, 22*kelvin);
  LH2->AddElement(elH, 2);
  fMaterialMap["lH2"] = LH2;

}

G4Material* Materials::GetMaterial(const G4String& name) {
  if (fMaterialMap.find(name) != fMaterialMap.end())
    return fMaterialMap[name];
  else {
    G4cerr << "Material " << name << " not found!" << G4endl;
    return nullptr;
  }
}

