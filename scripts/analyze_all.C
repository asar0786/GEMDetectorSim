void analyze_all() {
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
    TCut primary = "trackID==1";
    TCut secondary = "trackID!=1";

    Long64_t nPlane0 = t->GetEntries(plane0);
    Long64_t nPlane1 = t->GetEntries(plane1);

    std::cout << "\n========== PLANE ENTRY COUNTS ==========\n";
    std::cout << "Entries in plane 0: " << nPlane0 << "\n";
    std::cout << "Entries in plane 1: " << nPlane1 << "\n";
    std::cout << "========================================\n";

    TString pdffile = "analysis_all.pdf";
    TString open_pdf = pdffile + "[";
    TString close_pdf = pdffile + "]";

    // Start PDF output
    TCanvas* opener = new TCanvas("opener", "PDF start");
    opener->Print(open_pdf);

    // 2D Hit Maps
    TCanvas* c1 = new TCanvas("c1", "2D Hit Maps", 1600, 1200);
    c1->Divide(2,2);
    if (nPlane0 > 0) {
        c1->cd(1); t->Draw("y:x", plane0 + primary, "colz");
        c1->cd(3); t->Draw("y:x", plane0 + secondary, "colz");
    }
    if (nPlane1 > 0) {
        c1->cd(2); t->Draw("y:x", plane1 + primary, "colz");
        c1->cd(4); t->Draw("y:x", plane1 + secondary, "colz");
    }
    c1->Print(pdffile);

    // PDG ID histogram
    TCanvas* c2 = new TCanvas("c2", "Particle Types", 800, 600);
    TH1F* hpid = new TH1F("hpid", "Particle Type (PDG ID);PDG Code;Counts", 100, -2500, 2500);
    t->Draw("pdgID >> hpid", "", "hist");
    c2->Print(pdffile);

    // Angular deflection (only if both planes have data)
    if (nPlane0 > 0 && nPlane1 > 0) {
        TCanvas* c3 = new TCanvas("c3", "Angular Deflection", 1200, 600);
        c3->Divide(2,1);
        TTree* t0 = t->CopyTree("planeID==0");
        TTree* t1 = t->CopyTree("planeID==1");
        t0->BuildIndex("trackID");
        t1->BuildIndex("trackID");
        t1->AddFriend(t0, "p0");

        c3->cd(1);
        TH1F* dTheta = new TH1F("dTheta", "Angular Deflection #Delta#theta;#theta_{1}-#theta_{0} (rad);Counts", 100, -0.1, 0.1);
        t1->Draw("(theta - p0.theta) >> dTheta", "trackID==1", "hist");

        c3->cd(2);
        TH1F* dPhi = new TH1F("dPhi", "Angular Deflection #Delta#phi;#phi_{1}-#phi_{0} (rad);Counts", 100, -0.1, 0.1);
        t1->Draw("(phi - p0.phi) >> dPhi", "trackID==1", "hist");

        c3->Print(pdffile);
    }

    // Energy spectra
    TCanvas* c4 = new TCanvas("c4", "Energy Spectra", 1000, 600);
    c4->Divide(2,1);
    if (nPlane0 > 0) {
        c4->cd(1);
        TH1F* e0 = new TH1F("e0", "Energy Spectrum (Plane 0);Energy [MeV];Counts", 100, 0, 7000);
        t->Draw("energy >> e0", plane0, "hist");
        e0->SetLineColor(kBlue);
    }
    if (nPlane1 > 0) {
        c4->cd(2);
        TH1F* e1 = new TH1F("e1", "Energy Spectrum (Plane 1);Energy [MeV];Counts", 100, 0, 7000);
        t->Draw("energy >> e1", plane1, "hist");
        e1->SetLineColor(kRed);
    }
    c4->Print(pdffile);

    // Properly close PDF
    TCanvas* closer = new TCanvas("closer", "PDF end");
    closer->Print(close_pdf);

    // Final summary
    Long64_t total = t->GetEntries();
    Long64_t primaries = t->GetEntries("trackID==1");
    Long64_t secondaries = total - primaries;

    std::cout << "\n========= HIT SUMMARY =========\n";
    std::cout << "Total hits:      " << total << "\n";
    std::cout << "Primary hits:    " << primaries << "\n";
    std::cout << "Secondary hits:  " << secondaries << "\n";
    std::cout << "Saved to:        " << pdffile << "\n";
    std::cout << "===============================\n";
}

