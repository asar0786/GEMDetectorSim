#include "GEMSensitiveDetector.hh"
#include "RootOutputManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4EventManager.hh"

GEMSensitiveDetector::GEMSensitiveDetector(const G4String& name, G4int planeID)
    : G4VSensitiveDetector(name), fPlaneID(planeID) {}

G4bool GEMSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
    G4int eventid = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
    auto track = step->GetTrack();
    auto pos = step->GetPreStepPoint()->GetPosition();
    auto mom = track->GetMomentum();
    auto def = track->GetDefinition();

    HitRecord hit;
    hit.eventID = eventid;
    hit.trackID = track->GetTrackID();
    hit.parentID = track->GetParentID();
    hit.pdgID = def->GetPDGEncoding();
    hit.x = pos.x()/mm; hit.y = pos.y()/mm; hit.z = pos.z()/mm;
    hit.px = mom.x()/MeV; hit.py = mom.y()/MeV; hit.pz = mom.z()/MeV;
    hit.energy = track->GetKineticEnergy()/MeV;
    hit.theta = mom.theta(); hit.phi = mom.phi();
    hit.planeID = fPlaneID;
    hit.moduleID = fPlaneID - 100;

    auto touch = step->GetPreStepPoint()->GetTouchable();
    int modID = touch->GetCopyNumber();
    RootOutputManager::Instance()->RecordHit(hit, modID);
    return true;
}

