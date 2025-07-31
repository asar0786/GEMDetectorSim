#include "RootOutputManager.hh"
#include <cmath>  
#include <iostream>

RootOutputManager* RootOutputManager::fInstance = nullptr;

RootOutputManager::RootOutputManager() {
    fFile = new TFile("scoring.root", "RECREATE");
    fFile->cd();  
    fTree = new TTree("Hits", "All scoring plane hits");

    fTree->Branch("eventID", &eventID, "eventID/I");
    fTree->Branch("trackID", &trackID, "trackID/I");
    fTree->Branch("parentID", &parentID, "parentID/I");
    fTree->Branch("pdgID", &pdgID, "pdgID/I");
    fTree->Branch("planeID", &planeID, "planeID/I");
    fTree->Branch("x", &x, "x/D");
    fTree->Branch("y", &y, "y/D");
    fTree->Branch("z", &z, "z/D");
    fTree->Branch("px", &px, "px/D");
    fTree->Branch("py", &py, "py/D");
    fTree->Branch("pz", &pz, "pz/D");
    fTree->Branch("energy", &energy, "energy/D");
    fTree->Branch("theta", &theta, "theta/D");
    fTree->Branch("phi", &phi, "phi/D");
    fTree->Branch("moduleID", &moduleID, "moduleID/D");

}

RootOutputManager::~RootOutputManager() {
    Finalize();  // auto cleanup
}

void RootOutputManager::Finalize() {
    if (fFile && fFile->IsOpen()) {
        fFile->cd();
        std::cout << "[ROOT] Writing tree with " << fTree->GetEntries() << " entries" << std::endl;
        fTree->Write("", TObject::kOverwrite);
        fFile->Close();
        delete fFile;
        fFile = nullptr;
    }
}

RootOutputManager* RootOutputManager::Instance() {
    if (!fInstance) {
        fInstance = new RootOutputManager();
    }
    return fInstance;
}


void RootOutputManager::RecordHit(const HitRecord& hit, int moduleID) {
    if (!std::isfinite(hit.x) || !std::isfinite(hit.y) || !std::isfinite(hit.z) ||
        !std::isfinite(hit.px) || !std::isfinite(hit.py) || !std::isfinite(hit.pz) ||
        !std::isfinite(hit.energy) || !std::isfinite(hit.theta) || !std::isfinite(hit.phi)) {
        return; // skip bad values
    }

    eventID = hit.trackID;
    trackID = hit.trackID;
    parentID = hit.parentID;
    pdgID = hit.pdgID;
    planeID = hit.planeID;
    x = hit.x; y = hit.y; z = hit.z;
    px = hit.px; py = hit.py; pz = hit.pz;
    energy = hit.energy;
    theta = hit.theta;
    phi = hit.phi;
   this-> moduleID = moduleID;

    fTree->Fill();
}

