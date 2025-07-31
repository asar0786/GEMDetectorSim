#ifndef HitRecord_h
#define HitRecord_h 1

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

#endif

