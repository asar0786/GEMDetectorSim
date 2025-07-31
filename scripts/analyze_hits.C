void analyze_hits() {
    // Open file
    TFile* f = TFile::Open("../build/scoring.root");
    if (!f || f->IsZombie()) {
        std::cerr << "Error opening ROOT file\n";
        return;
    }

    // Load trees
    TTree* t0 = (TTree*)f->Get("plane0");
    TTree* t1 = (TTree*)f->Get("plane1");

    if (!t0 || !t1) {
        std::cerr << "One or both scoring planes not found.\n";
        return;
    }

    // Energy histograms
    TH1F* h0 = new TH1F("h0", "Energy Spectrum;Energy [MeV];Counts", 100, 0, 7000);
    TH1F* h1 = new TH1F("h1", "Energy Spectrum;Energy [MeV];Counts", 100, 0, 7000);

    t0->Draw("energy >> h0", "", "goff");
    t1->Draw("energy >> h1", "", "goff");

    h0->SetLineColor(kBlue);
    h1->SetLineColor(kRed);

    TCanvas* c1 = new TCanvas("c1", "Energy Comparison");
    h0->Draw();
    h1->Draw("SAME");
    auto legend = new TLegend(0.6, 0.7, 0.88, 0.88);
    legend->AddEntry(h0, "Plane 0 (near)", "l");
    legend->AddEntry(h1, "Plane 1 (5cm)", "l");
    legend->Draw();

    // Hit map from Plane 1
    TCanvas* c2 = new TCanvas("c2", "Hit Map Plane 1");
    t1->Draw("y:x", "", "colz");
}

