#ifndef RootOutputManager_h
#define RootOutputManager_h

#include "TFile.h"
#include "TTree.h"

struct HitRecord {
    int trackID;
    int parentID;
    int eventID;
    int pdgID;
    double x, y, z;
    double px, py, pz;
    double energy;
    double theta, phi;
    int planeID;
    int moduleID;
};

class RootOutputManager {
public:
    static RootOutputManager* Instance();
    void RecordHit(const HitRecord& hit, int moduleID);
    void Finalize();

private:
    RootOutputManager();
    ~RootOutputManager();

    static RootOutputManager* fInstance;
    TFile* fFile;
    TTree* fTree;
    HitRecord fHit;  // <-- this must exist!

    // Separate variables
    int eventID, trackID, parentID, pdgID, planeID;
    double x, y, z;
    double px, py, pz;
    double energy, theta, phi;
    int moduleID;
};

#endif

