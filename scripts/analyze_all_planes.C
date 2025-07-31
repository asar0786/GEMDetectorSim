//void analyze_all_planes(const char* filename = "../build/scoring.root") {
void analyze_all_planes(const char* filename = "../build/Elec_6GeV_0T_0deg_FR4_NoTarget.root") {
    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "Cannot open " << filename << std::endl;
        return;
    }

    TTree* t = (TTree*)f->Get("Hits");
    if (!t) {
        std::cerr << "Tree 'Hits' not found!" << std::endl;
        return;
    }

    TString pdfName = "analysis.pdf";
    TString pdfStart = pdfName + "[";
    TString pdfEnd = pdfName + "]";

    TCanvas* c = new TCanvas("c", "Plots", 1200, 800);
    c->Print(pdfStart);

    // Prepare summary histograms
    TH1F* hPrimPlane = new TH1F("hPrimPlane", "Primary Hits per Plane;Plane ID;Counts", 10, -0.5, 9.5);
    TH1F* hSecPlane  = new TH1F("hSecPlane", "Secondary Hits per Plane;Plane ID;Counts", 10, -0.5, 9.5);

    for (int plane = 0; plane < 10; ++plane) {
        TString cutPrimary = Form("planeID==%d && trackID==1", plane);
        TString cutSecondary = Form("planeID==%d && trackID!=1", plane);

        // Fill summary histograms
        Long64_t nPrim = t->GetEntries(cutPrimary);
        Long64_t nSec  = t->GetEntries(cutSecondary);
        if (plane == 11) continue;
        hPrimPlane->SetBinContent(plane + 1, nPrim);
        hSecPlane->SetBinContent(plane + 1, nSec);

        // 2D hit maps
        c->Clear();
c->Divide(2,1);

// Refined binning parameters
int binsX = 130, binsY = 130;
float xMin = -65, xMax = 65;
float yMin = -65, yMax = 65;

// Primary
c->cd(1);
TH2F* h2_primary = new TH2F(Form("h2_primary_plane%d", plane),
    Form("Primary Hits on Plane %d;x [mm];y [mm]", plane),
    binsX, xMin, xMax, binsY, yMin, yMax);
t->Project(h2_primary->GetName(), "y:x", cutPrimary);
h2_primary->Draw("colz");

// Secondary
c->cd(2);
TH2F* h2_secondary = new TH2F(Form("h2_secondary_plane%d", plane),
    Form("Secondary Hits on Plane %d;x [mm];y [mm]", plane),
    binsX, xMin, xMax, binsY, yMin, yMax);
t->Project(h2_secondary->GetName(), "y:x", cutSecondary);
h2_secondary->Draw("colz");

c->Print(pdfName);

//        c->Clear();
//        c->Divide(2,1);
//        c->cd(1);
//        t->Draw("y:x", cutPrimary, "colz");
//        gPad->SetTitle(Form("Primary Hits on Plane %d", plane));
//        c->cd(2);
//        t->Draw("y:x", cutSecondary, "colz");
//        gPad->SetTitle(Form("Secondary Hits on Plane %d", plane));
//        c->Print(pdfName);
//
        // Energy spectra
        c->Clear();
        c->Divide(2,1);
        c->cd(1);
        TH1F* ePrim = new TH1F("ePrim", Form("Primary Energy on Plane %d;Energy (MeV);Counts", plane), 100, 100, 12010);
        t->Draw("energy >> ePrim", cutPrimary, "hist");
        ePrim->SetLineColor(kBlue);

        c->cd(2);
        TH1F* eSec = new TH1F("eSec", Form("Secondary Energy on Plane %d;Energy (MeV);Counts", plane), 100, 0, 50);
        t->Draw("energy >> eSec", cutSecondary, "hist");
        eSec->SetLineColor(kRed);

        c->Print(pdfName);

        // Angular deflection θ and azimuthal φ
        c->Clear();
        c->Divide(2,1);
        c->cd(1);
        TH1F* hTheta = new TH1F("hTheta", Form("Theta (Primary) Plane %d;#theta [deg];Counts", plane), 100, 0, 1);
        t->Draw("theta*180/3.14159 >> hTheta", cutPrimary);
        hTheta->SetLineColor(kGreen);

        c->cd(2);
        TH1F* hPhi = new TH1F("hPhi", Form("Phi (Primary) Plane %d;#phi [deg];Counts", plane), 100, -180, 180);
        t->Draw("phi*180/3.14159 >> hPhi", cutPrimary);
        hPhi->SetLineColor(kMagenta);

        c->Print(pdfName);

        // 1D x and y
        c->Clear();
        c->Divide(2,2);
        c->cd(1);
        TH1F* hx = new TH1F("hx", Form("X Hit (Primary) Plane %d;x [mm];Counts", plane), 120, -60, 60);
        t->Draw("x >> hx", cutPrimary);
        hx->SetLineColor(kBlue);

        c->cd(2);
        TH1F* hy = new TH1F("hy", Form("Y Hit (Primary) Plane %d;y [mm];Counts", plane), 120, -60, 60);
        t->Draw("y >> hy", cutPrimary);
        hy->SetLineColor(kBlue);

        // Secondary X
c->cd(3);
TH1F* hxs = new TH1F(Form("hxs%d", plane), Form("X Hit (Secondary) Plane %d;x [mm];Counts", plane), 120, -60, 60);
t->Draw(Form("x >> hxs%d", plane), cutSecondary);
hxs->SetLineColor(kRed);

// Secondary Y
c->cd(4);
TH1F* hys = new TH1F(Form("hys%d", plane), Form("Y Hit (Secondary) Plane %d;y [mm];Counts", plane), 120, -60, 60);
t->Draw(Form("y >> hys%d", plane), cutSecondary);
hys->SetLineColor(kRed);
        c->Print(pdfName);

        delete ePrim; delete eSec;
        delete hTheta; delete hPhi;
        delete hx; delete hy;
    }

    // Final summary plot: hits per plane
    c->Clear();
    c->Divide(1,2);
    c->cd(1);
    hPrimPlane->SetFillColor(kBlue-7);
    hPrimPlane->SetMinimum(0);
    hPrimPlane->Draw("bar");
    gPad->SetTitle("Primary Hits per Plane");

    c->cd(2);
    hSecPlane->SetFillColor(kRed-7);
    hSecPlane->SetMinimum(0);
    hSecPlane->Draw("bar");
    gPad->SetTitle("Secondary Hits per Plane");

    c->Print(pdfName);
    c->Print(pdfEnd);

    f->Close();
}

void compare_two_planes(const char* file1 = "../build/Electron_6GeV_noshielding_Honeycomb.root", const char* file2 = "../build/Electron_6GeV_Alshielding_Honeycomb.root") {
    TFile* f1 = TFile::Open(file1);
    TFile* f2 = TFile::Open(file2);
    if (!f1 || !f2 || f1->IsZombie() || f2->IsZombie()) {
        std::cerr << "Cannot open one of the files.\n";
        return;
    }

    TTree* t1 = (TTree*)f1->Get("Hits");
    TTree* t2 = (TTree*)f2->Get("Hits");
    if (!t1 || !t2) {
        std::cerr << "One of the trees 'Hits' not found.\n";
        return;
    }

    TString pdfName = "comparison.pdf";
    TString pdfStart = pdfName + "[";
    TString pdfEnd = pdfName + "]";

    TCanvas* c = new TCanvas("c", "Comparison", 1200, 800);
    c->Print(pdfStart);

    for (int plane = 0; plane < 10; ++plane) {
        TString cut1 = Form("planeID==%d && trackID==1", plane);
        TString cut2 = Form("planeID==%d && trackID!=1 && pdgID==11", plane);

        // Energy
        c->Clear();
        c->Divide(2,1);
        c->cd(1);
        TH1F* h1 = new TH1F("h1", Form("Primary Energy (Plane %d);Energy (MeV);Counts", plane), 100, 5950, 6010);
        t1->Draw("energy >> h1", cut1, "hist");
        h1->SetLineColor(kBlue);

        TH1F* h2 = new TH1F("h2", "", 100, 5950, 6010);
        t2->Draw("energy >> h2", cut2, "hist");
        h2->SetLineColor(kRed);
        h2->Draw("hist same");
        h1->Draw("hist same");
        gPad->BuildLegend();

        c->cd(2);
        TH1F* hs1 = new TH1F("hs1", Form("Secondary Energy (Plane %d);Energy (MeV);Counts", plane), 100, 0, 50);
        t1->Draw("energy >> hs1", Form("planeID==%d && trackID!=1", plane));
        hs1->SetLineColor(kBlue);

        TH1F* hs2 = new TH1F("hs2", "", 100, 0, 50);
        t2->Draw("energy >> hs2", Form("planeID==%d && trackID!=1", plane));
        hs2->SetLineColor(kRed);
        hs2->Draw("hist same");
        hs1->Draw("hist same");
        gPad->BuildLegend();

        c->Print(pdfName);

        // 1D X hits
        c->Clear(); c->Divide(2,1);
        c->cd(1);
        TH1F* hx1 = new TH1F("hx1", Form("X (Primary) Plane %d;x [mm];Counts", plane), 120, -60, 60);
        TH1F* hx2 = new TH1F("hx2", "", 120, -60, 60);
        t1->Draw("x >> hx1", cut1);
        t2->Draw("x >> hx2", cut2);
        hx1->SetLineColor(kBlue); hx2->SetLineColor(kRed);
        hx1->Draw(); hx2->Draw("same"); gPad->BuildLegend();

        c->cd(2);
        TH1F* hxs1 = new TH1F("hxs1", Form("X (Secondary) Plane %d;x [mm];Counts", plane), 120, -60, 60);
        TH1F* hxs2 = new TH1F("hxs2", "", 120, -60, 60);
        t1->Draw("x >> hxs1", Form("planeID==%d && trackID!=1", plane));
        t2->Draw("x >> hxs2", Form("planeID==%d && trackID!=1", plane));
        hxs1->SetLineColor(kBlue); hxs2->SetLineColor(kRed);
        hxs1->Draw(); hxs2->Draw("same"); gPad->BuildLegend();

        c->Print(pdfName);

        // Theta and Phi
        c->Clear(); c->Divide(2,1);
        c->cd(1);
        TH1F* th1 = new TH1F("th1", Form("#theta (Primary) Plane %d;#theta [deg];Counts", plane), 100, 0, 1.0);
        TH1F* th2 = new TH1F("th2", "", 100, 0, 1.0);
        t1->Draw("theta*180/3.14159 >> th1", cut1);
        t2->Draw("theta*180/3.14159 >> th2", cut2);
        th1->SetLineColor(kBlue); th2->SetLineColor(kRed);
        th1->Draw(); th2->Draw("same"); gPad->BuildLegend();

        c->cd(2);
        TH1F* ph1 = new TH1F("ph1", Form("#phi (Primary) Plane %d;#phi [deg];Counts", plane), 100, -180, 180);
        TH1F* ph2 = new TH1F("ph2", "", 100, -180, 180);
        t1->Draw("phi*180/3.14159 >> ph1", cut1);
        t2->Draw("phi*180/3.14159 >> ph2", cut2);
        ph1->SetLineColor(kBlue); ph2->SetLineColor(kRed);
        ph1->Draw(); ph2->Draw("same"); gPad->BuildLegend();

        c->Print(pdfName);
    }

    c->Print(pdfEnd);
    f1->Close(); f2->Close();
}

