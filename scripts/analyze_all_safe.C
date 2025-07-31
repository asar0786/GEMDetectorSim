void analyze_all_safe() {
    TFile* f = TFile::Open("../build/scoring.root");
    if (!f || f->IsZombie()) {
        std::cerr << "ERROR: Cannot open scoring.root\n";
        return;
    }

    TTree* t = (TTree*)f->Get("Hits");
    if (!t) {
        std::cerr << "ERROR: Tree 'Hits' not found\n";
        return;
    }

    TCut plane0 = "planeID==0";
    TCut plane1 = "planeID==1";
    TCut plane2 = "planeID==2";
    TCut primary = "trackID==1";
    TCut secondary = "trackID!=1";

    Long64_t nPlane0 = t->GetEntries("planeID==0");
    Long64_t nPlane1 = t->GetEntries("planeID==1");

    std::cout << "\n========== PLANE ENTRY COUNTS ==========" << std::endl;
    std::cout << "Entries in plane 0: " << nPlane0 << std::endl;
    std::cout << "Entries in plane 1: " << nPlane1 << std::endl;
    std::cout << "========================================" << std::endl;

    TString pdffile = "analysis.pdf";
    TString open_pdf = pdffile + "[";
    TString close_pdf = pdffile + "]";

    TCanvas* dummy = new TCanvas();
    dummy->Print(open_pdf, "pdf");

    TCanvas* c1 = new TCanvas("c1", "2D Hit Maps", 1600, 1200);
    c1->Divide(2,2);
    bool has_c1 = false;

    // Define binning and range (e.g. 5 mm bins over 13×13 cm area)
const int nBins = 140;       // 13 cm / 0.5 cm
const double rangeMin = -70.5;
const double rangeMax = 70.5;
    if (nPlane0 > 0) {
    // Primary Hits on Plane 0
    c1->cd(1);
    TH2D* hPrim0 = new TH2D("hPrim0", "Primary Hits on Plane 0;x [mm];y [mm]",
                            nBins, rangeMin, rangeMax, nBins, rangeMin, rangeMax);
    t->Draw("y:x >> hPrim0", plane0 + primary, "colz");
    has_c1 = true;

    // Secondary Hits on Plane 0
    c1->cd(3);
    TH2D* hSec0 = new TH2D("hSec0", "Secondary Hits on Plane 0;x [mm];y [mm]",
                           nBins, rangeMin, rangeMax, nBins, rangeMin, rangeMax);
    t->Draw("y:x >> hSec0", plane0 + secondary, "colz");
}
    if (nPlane0 > 1) {
    // Primary Hits on Plane 1
    c1->cd(2);
    TH2D* hPrim1 = new TH2D("hPrim1", "Primary Hits on Plane 1;x [mm];y [mm]",
                            nBins, rangeMin, rangeMax, nBins, rangeMin, rangeMax);
    t->Draw("y:x >> hPrim1", plane1 + primary, "colz");
    has_c1 = true;

    // Secondary Hits on Plane 1
    c1->cd(4);
    TH2D* hSec1 = new TH2D("hSec1", "All Hits on Plane 1;x [mm];y [mm]",
                           nBins, rangeMin, rangeMax, nBins, rangeMin, rangeMax);
    t->Draw("y:x >> hSec1", plane1, "colz");
}
//    if (nPlane0 > 0) {
//        c1->cd(1);
//        t->Draw("y:x", plane0 + primary, "colz");
//        gPad->SetTitle("Primary Hits on Plane 0");
//        has_c1 = true;
//
//        c1->cd(3);
//        t->Draw("y:x", plane0 + secondary, "colz");
//        gPad->SetTitle("Secondary Hits on Plane 0");
//    }

//    if (nPlane1 > 0) {
//        c1->cd(2);
//        t->Draw("y:x", plane1 + primary, "colz");
//        gPad->SetTitle("Primary Hits on Plane 1");
//        has_c1 = true;
//
//        c1->cd(4);
//        t->Draw("y:x", plane1 + secondary, "colz");
//        gPad->SetTitle("Secondary Hits on Plane 1");
//    }

    if (has_c1) c1->Print(pdffile);
//----------RO as sensitive detector------------    
    TCanvas* c7 = new TCanvas("c1", "2D Hit Maps", 1600, 1200);
    c7->Divide(2,1);
    bool has_c7 = false;
    if (nPlane0 > 0) {
    // Primary Hits on Plane 0
    c7->cd(1);
    TH2D* hPrim2 = new TH2D("hPrim2", "Primary Hits on Plane RO;x [mm];y [mm]",
                            nBins, rangeMin, rangeMax, nBins, rangeMin, rangeMax);
    t->Draw("y:x >> hPrim2", plane2 + primary, "colz");
    has_c7 = true;

    // Secondary Hits on Plane 0
    c7->cd(2);
    TH2D* hSec2 = new TH2D("hSec2", "Secondary Hits on Plane RO;x [mm];y [mm]",
                           nBins, rangeMin, rangeMax, nBins, rangeMin, rangeMax);
    t->Draw("y:x >> hSec2", plane2 + secondary, "colz");
}
//    if (nPlane0 > 1) {
//    // Primary Hits on Plane 1
//    c7->cd(2);
//    TH2D* hPrim1 = new TH2D("hPrim1", "Primary Hits on Plane 1;x [mm];y [mm]",
//                            nBins, rangeMin, rangeMax, nBins, rangeMin, rangeMax);
//    t->Draw("y:x >> hPrim1", plane1 + primary, "colz");
//    has_c7 = true;
//
//    // Secondary Hits on Plane 1
//    c7->cd(4);
//    TH2D* hSec1 = new TH2D("hSec1", "Secondary Hits on Plane 1;x [mm];y [mm]",
//                           nBins, rangeMin, rangeMax, nBins, rangeMin, rangeMax);
//    t->Draw("y:x >> hSec1", plane1 + secondary, "colz");
//}

    if (has_c7) c7->Print(pdffile);
//-------------------------------------------------------------------------------
    TCanvas* c2 = new TCanvas("c2", "Particle Types", 800, 600);
    TH1F* hpid = new TH1F("hpid", "Particle Type (PDG ID);PDG Code;Counts", 50, 0, 50);
    t->Draw("pdgID >> hpid", "", "hist");
    if (hpid->GetEntries() > 0) c2->Print(pdffile);

//////------------Energy spectra--------
    TCanvas* c4 = new TCanvas("c4", "Energy Spectra", 1000, 600);
    c4->Divide(2,1);
    bool has_c4 = false;

    if (nPlane0 > 0) {
        c4->cd(1);
        TH1F* e0 = new TH1F("e0", "Energy Spectrum Plane 0(primary);Energy (MeV);Counts", 1000, 5950, 6005);
        t->Draw("energy >> e0", plane0 + primary, "hist");
        e0->SetLineColor(kBlue);
        has_c4 = true;
    }

    if (nPlane1 > 0) {
        c4->cd(2);
        TH1F* e1 = new TH1F("e1", "Energy Spectrum Plane 0(secondary);Energy (MeV);Counts", 1000, 0, 50);
        t->Draw("energy >> e1", plane1 + secondary, "hist");
        e1->SetLineColor(kRed);
        has_c4 = true;
    }

    if (has_c4) c4->Print(pdffile);
        // Angular deflection from original direction (0,0,1) using momentum on plane 1
    if (nPlane1 > 0) {
        TCanvas* c5 = new TCanvas("c5", "Deflection from Beam Axis", 1200, 600);
        c5->Divide(2,1);

        c5->cd(1);
        TH1F* hThetaDeflect = new TH1F("hThetaDeflect", "Deflection Angle from z-axis;#theta (deg);Counts", 100, 0, 0.2);
        t->Draw("acos(pz/sqrt(px*px + py*py + pz*pz))*180.0/3.1415926 >> hThetaDeflect", "planeID==1 && trackID==1", "hist");

        c5->cd(2);
        TH1F* hPhiDeflect = new TH1F("hPhiDeflect", "Azimuthal Angle from Beam;#phi (deg);Counts", 100, -180, 180);
        t->Draw("atan2(py,px)*180.0/3.1415926 >> hPhiDeflect", "planeID==1 && trackID==1", "hist");

        c5->Print(pdffile);
    }
        // 1D hit position distributions on Plane 0 for primary particles
    if (nPlane0 > 0) {
        TCanvas* c6 = new TCanvas("c6", "1D Hit Distributions on Plane 0", 1200, 600);
        c6->Divide(2,1);

        c6->cd(1);
        TH1F* hX_plane0 = new TH1F("hX_plane0", "Hit X on Plane 0 (Primary);X [mm];Counts", 160, -80.5, 80.0);
        t->Draw("x >> hX_plane0", "planeID==0 && trackID==1", "hist");

        c6->cd(2);
        TH1F* hY_plane0 = new TH1F("hY_plane0", "Hit Y on Plane 0 (Primary);Y [mm];Counts", 160, -80.5, 80.5);
        t->Draw("y >> hY_plane0", "planeID==0 && trackID==1", "hist");

        c6->Print(pdffile);
    }



    TCanvas* cFinal = new TCanvas("cFinal", "", 100, 100);
    cFinal->Print(close_pdf, "pdf");
    delete cFinal;

    Long64_t total = t->GetEntries();
    Long64_t primaries = t->GetEntries("trackID==1");
    Long64_t secondaries = total - primaries;

    std::cout << "\n========= HIT SUMMARY =========" << std::endl;
    std::cout << "Total hits:      " << total << std::endl;
    std::cout << "Primary hits:    " << primaries << std::endl;
    std::cout << "Secondary hits:  " << secondaries << std::endl;
    std::cout << "Saved to:        " << pdffile << std::endl;
    std::cout << "===============================" << std::endl;
    //---------Primaries calculation--------
    Long64_t nTotalPrim = 1000000;
Long64_t nPlane0_prim = t->GetEntries("trackID == 1 && planeID == 0");
Long64_t nPlane1_prim = t->GetEntries("trackID == 1 && planeID == 1");

Long64_t nBlocked = nTotalPrim - nPlane1_prim;
std::cout << "\n===== PRIMARY TRACKING =====" << std::endl;
std::cout << "Primaries generated   : " << nTotalPrim << std::endl;
std::cout << "Reached Plane 0       : " << nPlane0_prim << std::endl;
std::cout << "Reached Plane 1       : " << nPlane1_prim << std::endl;
std::cout << "Blocked / Absorbed    : " << nBlocked << std::endl;
std::cout << "============================\n" << std::endl;

}

