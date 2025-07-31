#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h" // Include for 2D histograms
#include "TLegend.h"
#include "TString.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TPad.h"
#include "TColor.h"
#include "TMath.h" // For TMath::Pi()
#include "THStack.h" // For stacked histograms

#include <iostream>

void compare_two_planes(const char* file1 = "../build/Electron_10GeV_18degree.root", const char* file2 = "../build/Electron_10GeV_18degree_cu_shieldind.root") {
    // Set ROOT to batch mode (no pop-up canvases) for PDF generation
    gROOT->SetBatch(kTRUE);
    // Do not show statistics box on plots by default
    gStyle->SetOptStat(0);
    // Set a nice color palette for 2D plots (optional, but improves visualization)
    gStyle->SetPalette(kRainBow);

    // Open the input ROOT files
    TFile* f1 = TFile::Open(file1);
    TFile* f2 = TFile::Open(file2);

    // Check if files were opened successfully
    if (!f1 || !f2 || f1->IsZombie() || f2->IsZombie()) {
        std::cerr << "Error: Cannot open one of the ROOT files.\n";
        return;
    }

    // Get the "Hits" TTree from each file
    TTree* t1 = (TTree*)f1->Get("Hits");
    TTree* t2 = (TTree*)f2->Get("Hits");

    // Check if TTrees were found
    if (!t1 || !t2) {
        std::cerr << "Error: One of the trees 'Hits' not found in the files.\n";
        f1->Close();
        f2->Close();
        return;
    }

    // Define PDF output file name
    TString pdfName = "comparison.pdf";
    TString pdfStart = pdfName + "["; // Start PDF
    TString pdfEnd = pdfName + "]";   // End PDF

    // Create a new canvas for plotting
    TCanvas* c = new TCanvas("c", "Comparison", 1200, 800);
    c->Print(pdfStart); // Open the PDF file

    // --- Primary and Secondary Hits per Plane (Summary Plot) ---
    // Histograms to store total primary and secondary hits per plane for each file
    TH1F* hPrim1 = new TH1F("hPrim1", "Primary Hits per Plane;Plane ID;Counts", 9, -0.5, 8.5);
    TH1F* hPrim2 = new TH1F("hPrim2", "", 9, -0.5, 8.5); // Title will be set by legend
    TH1F* hSec1  = new TH1F("hSec1", "Secondary Hits per Plane;Plane ID;Counts", 9, -0.5, 8.5);
    TH1F* hSec2  = new TH1F("hSec2", "", 9, -0.5, 8.5); // Title will be set by legend

    // Loop through each plane to fill the summary histograms
    for (int plane = 0; plane < 9; ++plane) {
        // Define cuts for primary (trackID==1) and secondary (trackID!=1) hits
        TString cut1 = Form("planeID==%d && trackID==1", plane);
        TString cut2 = Form("planeID==%d && trackID!=1", plane);

        // Fill primary and secondary hit counts for each plane
        // Bin indexing starts from 1, so plane + 1
        hPrim1->SetBinContent(plane + 1, t1->GetEntries(cut1));
        hPrim2->SetBinContent(plane + 1, t2->GetEntries(cut1));
        hSec1->SetBinContent(plane + 1, t1->GetEntries(cut2));
        hSec2->SetBinContent(plane + 1, t2->GetEntries(cut2));
    }

    // Set drawing styles for primary hits histograms
    hPrim1->SetLineColor(kBlue);
    hPrim1->SetFillColorAlpha(kBlue, 0.3); // Semi-transparent fill
    hPrim1->GetYaxis()->SetTitle("Counts");
    hPrim1->GetXaxis()->SetTitle("Plane ID");

    hPrim2->SetLineColor(kRed);
    hPrim2->SetLineStyle(2);       // Dashed line
    hPrim2->SetMarkerStyle(20);    // Circle markers
    hPrim2->GetYaxis()->SetTitle("Counts");
    hPrim2->GetXaxis()->SetTitle("Plane ID");

    // Set drawing styles for secondary hits histograms
    hSec1->SetLineColor(kBlue);
    hSec1->SetFillColorAlpha(kBlue, 0.3);
    hSec1->GetYaxis()->SetTitle("Counts");
    hSec1->GetXaxis()->SetTitle("Plane ID");

    hSec2->SetLineColor(kRed);
    hSec2->SetLineStyle(2);
    hSec2->SetMarkerStyle(20);
    hSec2->GetYaxis()->SetTitle("Counts");
    hSec2->GetXaxis()->SetTitle("Plane ID");

    // Adjust Y-axis range for hit count plots to fit all data
    double maxPrim = TMath::Max(hPrim1->GetMaximum(), hPrim2->GetMaximum());
    hPrim1->GetYaxis()->SetRangeUser(0, maxPrim * 1.1); // 10% buffer
    hPrim2->GetYaxis()->SetRangeUser(0, maxPrim * 1.1);

    double maxSec = TMath::Max(hSec1->GetMaximum(), hSec2->GetMaximum());
    hSec1->GetYaxis()->SetRangeUser(0, maxSec * 1.1);
    hSec2->GetYaxis()->SetRangeUser(0, maxSec * 1.1);

    // Clear canvas and divide for primary and secondary hit count plots
    c->Clear();
    c->Divide(1,2); // 1 column, 2 rows

    // Draw Primary Hits per Plane
    c->cd(1);
    hPrim1->Draw("HIST"); // Draw with histogram style
    hPrim2->Draw("P SAME"); // Draw points on the same pad
    TLegend* legPrim = new TLegend(0.15, 0.75, 0.45, 0.88); // Define legend position
    legPrim->AddEntry(hPrim1, "Without shielding", "lf"); // 'lf' for line and fill style
    legPrim->AddEntry(hPrim2, "With shielding", "lp");   // 'lp' for line and point style
    legPrim->Draw();
    hPrim1->SetTitle("Primary Hits per Plane"); // Set title explicitly after drawing

    c->cd(2);
    hSec1->Draw("HIST");
    hSec2->Draw("P SAME");
    TLegend* legSec = new TLegend(0.15, 0.75, 0.45, 0.88);
    legSec->AddEntry(hSec1, "Without shielding", "lf");
    legSec->AddEntry(hSec2, "With shielding", "lp");
    legSec->Draw();
    hSec1->SetTitle("Secondary Hits per Plane");
    c->Print(pdfName); // Print this page to PDF

    // Loop through each plane for detailed per-plane plots
    for (int plane = 0; plane < 9; ++plane) {
        TString cut1 = Form("planeID==%d && trackID==1", plane);    // Cut for primary hits in current plane
        TString cut2 = Form("planeID==%d && trackID!=1", plane);    // Cut for secondary hits in current plane

        // --- Energy Histograms (Primary and Secondary) ---
        c->Clear();
        c->Divide(2,1); // 2 columns, 1 row

        // Primary Energy
        c->cd(1);
        // Create histograms with unique names for each plane
        TH1F* h1_energy = new TH1F(Form("h1_energy_plane%d", plane), Form("Primary Energy (Plane %d);Energy (MeV);Counts", plane), 100, 3000, 6010);
        t1->Draw(Form("energy >> %s", h1_energy->GetName()), cut1, "HIST"); // Draw to the named histogram
        h1_energy->SetLineColor(kBlue);
        h1_energy->GetXaxis()->SetTitle("Energy (MeV)");
        h1_energy->GetYaxis()->SetTitle("Counts");

        TH1F* h2_energy = new TH1F(Form("h2_energy_plane%d", plane), "", 100, 3000, 6010);
        t2->Draw(Form("energy >> %s", h2_energy->GetName()), cut1, "HIST SAME"); // Draw to the named histogram, same pad
        h2_energy->SetLineColor(kRed);

        TLegend* leg_prim_energy = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_prim_energy->AddEntry(h1_energy, "Without shielding", "l");
        leg_prim_energy->AddEntry(h2_energy, "With shielding", "l");
        leg_prim_energy->Draw();

        // Secondary Energy
        c->cd(2);
        TH1F* hs1_energy = new TH1F(Form("hs1_energy_plane%d", plane), Form("Secondary Energy (Plane %d);Energy (MeV);Counts", plane), 100, 0, 50);
        t1->Draw(Form("energy >> %s", hs1_energy->GetName()), cut2, "HIST");
        hs1_energy->SetLineColor(kBlue);
        hs1_energy->GetXaxis()->SetTitle("Energy (MeV)");
        hs1_energy->GetYaxis()->SetTitle("Counts");

        TH1F* hs2_energy = new TH1F(Form("hs2_energy_plane%d", plane), "", 100, 0, 50);
        t2->Draw(Form("energy >> %s", hs2_energy->GetName()), cut2, "HIST SAME");
        hs2_energy->SetLineColor(kRed);

        TLegend* leg_sec_energy = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_sec_energy->AddEntry(hs1_energy, "Without shielding", "l");
        leg_sec_energy->AddEntry(hs2_energy, "With shielding", "l");
        leg_sec_energy->Draw();

        c->Print(pdfName); // Print this page to PDF

        // --- 1D X hits and 2D X-Y Hits ---
        c->Clear();
        c->Divide(2,2); // 2 columns, 2 rows for 1D X and 2D X-Y plots

        // Primary X (1D)
        c->cd(1);
        TH1F* hx1 = new TH1F(Form("hx1_plane%d", plane), Form("X (Primary) Plane %d;X [mm];Counts", plane), 120, -60, 60);
        t1->Draw(Form("x >> %s", hx1->GetName()), cut1, "HIST");
        hx1->SetLineColor(kBlue);
        hx1->GetXaxis()->SetTitle("X [mm]");
        hx1->GetYaxis()->SetTitle("Counts");

        TH1F* hx2 = new TH1F(Form("hx2_plane%d", plane), "", 120, -60, 60);
        t2->Draw(Form("x >> %s", hx2->GetName()), cut1, "HIST SAME");
        hx2->SetLineColor(kRed);
        TLegend* leg_prim_x = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_prim_x->AddEntry(hx1, "Without shielding", "l");
        leg_prim_x->AddEntry(hx2, "With shielding", "l");
        leg_prim_x->Draw();

        // Secondary X (1D)
        c->cd(2);
        TH1F* hxs1 = new TH1F(Form("hxs1_plane%d", plane), Form("X (Secondary) Plane %d;X [mm];Counts", plane), 120, -60, 60);
        t1->Draw(Form("x >> %s", hxs1->GetName()), cut2, "HIST");
        hxs1->SetLineColor(kBlue);
        hxs1->GetXaxis()->SetTitle("X [mm]");
        hxs1->GetYaxis()->SetTitle("Counts");

        TH1F* hxs2 = new TH1F(Form("hxs2_plane%d", plane), "", 120, -60, 60);
        t2->Draw(Form("x >> %s", hxs2->GetName()), cut2, "HIST SAME");
        hxs2->SetLineColor(kRed);
        TLegend* leg_sec_x = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_sec_x->AddEntry(hxs1, "Without shielding", "l");
        leg_sec_x->AddEntry(hxs2, "With shielding", "l");
        leg_sec_x->Draw();

        // Primary X-Y (2D)
        c->cd(3); // New pad for 2D plot
        TH2F* hxy1 = new TH2F(Form("hxy1_plane%d", plane), Form("XY (Primary) Plane %d;X [mm];Y [mm]", plane), 100, -60, 60, 100, -60, 60);
        t1->Draw(Form("y:x >> %s", hxy1->GetName()), cut1, "COLZ"); // "COLZ" for 2D color plot with palette
        hxy1->GetXaxis()->SetTitle("X [mm]");
        hxy1->GetYaxis()->SetTitle("Y [mm]");

        // Secondary X-Y (2D)
        c->cd(4); // New pad for 2D plot
        TH2F* hxy2 = new TH2F(Form("hxy2_plane%d", plane), Form("XY (Secondary) Plane %d;X [mm];Y [mm]", plane), 100, -60, 60, 100, -60, 60);
        t2->Draw(Form("y:x >> %s", hxy2->GetName()), cut2, "COLZ");
        hxy2->GetXaxis()->SetTitle("X [mm]");
        hxy2->GetYaxis()->SetTitle("Y [mm]");

        c->Print(pdfName); // Print this page to PDF

        // --- Theta and Phi (Primary and Secondary) ---
        c->Clear();
        c->Divide(2,2); // 2 columns, 2 rows for 4 plots

        // Primary Theta
        c->cd(1);
        TH1F* th1 = new TH1F(Form("th1_plane%d", plane), Form("#theta (Primary) Plane %d;#theta [deg];Counts", plane), 100, 0, 20.0);
        t1->Draw(Form("theta*180/TMath::Pi() >> %s", th1->GetName()), cut1, "HIST");
        th1->SetLineColor(kBlue);
        th1->GetXaxis()->SetTitle("#theta [deg]");
        th1->GetYaxis()->SetTitle("Counts");

        TH1F* th2 = new TH1F(Form("th2_plane%d", plane), "", 100, 0, 20.0);
        t2->Draw(Form("theta*180/TMath::Pi() >> %s", th2->GetName()), cut1, "HIST SAME");
        th2->SetLineColor(kRed);
        TLegend* leg_prim_theta = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_prim_theta->AddEntry(th1, "Without shielding", "l");
        leg_prim_theta->AddEntry(th2, "With shielding", "l");
        leg_prim_theta->Draw();

        // Primary Phi
        c->cd(2);
        TH1F* ph1 = new TH1F(Form("ph1_plane%d", plane), Form("#phi (Primary) Plane %d;#phi [deg];Counts", plane), 100, -180, 180);
        t1->Draw(Form("phi*180/TMath::Pi() >> %s", ph1->GetName()), cut1, "HIST");
        ph1->SetLineColor(kBlue);
        ph1->GetXaxis()->SetTitle("#phi [deg]");
        ph1->GetYaxis()->SetTitle("Counts");

        TH1F* ph2 = new TH1F(Form("ph2_plane%d", plane), "", 100, -180, 180);
        t2->Draw(Form("phi*180/TMath::Pi() >> %s", ph2->GetName()), cut1, "HIST SAME");
        ph2->SetLineColor(kRed);
        TLegend* leg_prim_phi = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_prim_phi->AddEntry(ph1, "Without shielding", "l");
        leg_prim_phi->AddEntry(ph2, "With shielding", "l");
        leg_prim_phi->Draw();

        // Secondary Theta
        c->cd(3); // Corrected pad for secondary theta
        TH1F* ths1 = new TH1F(Form("ths1_plane%d", plane), Form("#theta (Secondary) Plane %d;#theta [deg];Counts", plane), 100, 0, 20.0);
        t1->Draw(Form("theta*180/TMath::Pi() >> %s", ths1->GetName()), cut2, "HIST");
        ths1->SetLineColor(kBlue);
        ths1->GetXaxis()->SetTitle("#theta [deg]");
        ths1->GetYaxis()->SetTitle("Counts");

        TH1F* ths2 = new TH1F(Form("ths2_plane%d", plane), "", 100, 0, 20.0);
        t2->Draw(Form("theta*180/TMath::Pi() >> %s", ths2->GetName()), cut2, "HIST SAME");
        ths2->SetLineColor(kRed);
        TLegend* leg_sec_theta = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_sec_theta->AddEntry(ths1, "Without shielding", "l");
        leg_sec_theta->AddEntry(ths2, "With shielding", "l");
        leg_sec_theta->Draw();

        // Secondary Phi
        c->cd(4); // Corrected pad for secondary phi
        TH1F* phs1 = new TH1F(Form("phs1_plane%d", plane), Form("#phi (Secondary) Plane %d;#phi [deg];Counts", plane), 100, -180, 180);
        t1->Draw(Form("phi*180/TMath::Pi() >> %s", phs1->GetName()), cut2, "HIST");
        phs1->SetLineColor(kBlue);
        phs1->GetXaxis()->SetTitle("#phi [deg]");
        phs1->GetYaxis()->SetTitle("Counts");

        TH1F* phs2 = new TH1F(Form("phs2_plane%d", plane), "", 100, -180, 180);
        t2->Draw(Form("phi*180/TMath::Pi() >> %s", phs2->GetName()), cut2, "HIST SAME");
        phs2->SetLineColor(kRed);
        TLegend* leg_sec_phi = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_sec_phi->AddEntry(phs1, "Without shielding", "l");
        leg_sec_phi->AddEntry(phs2, "With shielding", "l");
        leg_sec_phi->Draw();

        c->Print(pdfName); // Print this page to PDF

        // Delete histograms and legends created within the loop to prevent memory leaks
        delete h1_energy; delete h2_energy;
        delete hs1_energy; delete hs2_energy;
        delete hx1; delete hx2;
        delete hxs1; delete hxs2;
        delete hxy1; delete hxy2; // Delete 2D histograms
        delete th1; delete th2;
        delete ph1; delete ph2;
        delete ths1; delete ths2;
        delete phs1; delete phs2;

        delete leg_prim_energy; delete leg_sec_energy;
        delete leg_prim_x; delete leg_sec_x;
        delete leg_prim_theta; delete leg_prim_phi;
        delete leg_sec_theta; delete leg_sec_phi;
    }

    // --- Overlaid Secondary Particle Composition Plots ---
    // This section assumes a 'PDGcode' branch exists in your TTree for particle identification.
    // Common PDG codes: Electron (11), Positron (-11), Proton (2212), Gamma (22)

    c->Clear();
    c->Divide(1,2); // One column, two rows for overlaid plots for file1 and file2

    // --- Overlaid Plot for File 1 (Without Shielding) ---
    c->cd(1);
    // Create histograms for each secondary particle type for File 1
    TH1F* h_e_f1 = new TH1F("h_e_f1", "Electrons (Without Shielding);Plane ID;Counts", 9, -0.5, 8.5);
    TH1F* h_p_f1 = new TH1F("h_p_f1", "Protons (Without Shielding);Plane ID;Counts", 9, -0.5, 8.5);
    TH1F* h_g_f1 = new TH1F("h_g_f1", "Gammas (Without Shielding);Plane ID;Counts", 9, -0.5, 8.5);
    TH1F* h_pos_f1 = new TH1F("h_pos_f1", "Positrons (Without Shielding);Plane ID;Counts", 9, -0.5, 8.5);

    // Set line colors and styles for overlaying
    h_e_f1->SetLineColor(kRed);     h_e_f1->SetLineStyle(1); h_e_f1->SetLineWidth(2);
    h_p_f1->SetLineColor(kGreen+2);  h_p_f1->SetLineStyle(2); h_p_f1->SetLineWidth(2);
    h_g_f1->SetLineColor(kBlue);    h_g_f1->SetLineStyle(3); h_g_f1->SetLineWidth(2);
    h_pos_f1->SetLineColor(kOrange+7); h_pos_f1->SetLineStyle(4); h_pos_f1->SetLineWidth(2);

    // Loop through planes to fill the particle type histograms for File 1
    for (int plane = 0; plane < 9; ++plane) {
        TString cut_e = Form("planeID==%d && trackID!=1 && pdgID==11", plane);
        TString cut_p = Form("planeID==%d && trackID!=1 && pdgID==2212", plane);
        TString cut_g = Form("planeID==%d && trackID!=1 && pdgID==22", plane);
        TString cut_pos = Form("planeID==%d && trackID!=1 && pdgID==-11", plane);
        h_e_f1->SetBinContent(plane + 1, t1->GetEntries(cut_e));
        h_p_f1->SetBinContent(plane + 1, t1->GetEntries(cut_p));
        h_g_f1->SetBinContent(plane + 1, t1->GetEntries(cut_g));
        h_pos_f1->SetBinContent(plane + 1, t1->GetEntries(cut_pos));
    }

    // Determine the maximum Y value across all overlaid histograms for proper scaling
    double max_overlay_f1 = TMath::Max(h_e_f1->GetMaximum(), h_p_f1->GetMaximum());
    max_overlay_f1 = TMath::Max(max_overlay_f1, h_g_f1->GetMaximum());
    max_overlay_f1 = TMath::Max(max_overlay_f1, h_pos_f1->GetMaximum());

    // Draw the first histogram, then overlay others
    h_e_f1->GetYaxis()->SetRangeUser(0, max_overlay_f1 * 1.2); // Set Y-axis range
    h_e_f1->SetTitle("Secondary Particle Composition (Without Shielding);Plane ID;Counts");
    h_e_f1->Draw("HIST");
    h_p_f1->Draw("HIST SAME");
    h_g_f1->Draw("HIST SAME");
    h_pos_f1->Draw("HIST SAME");

    TLegend* leg_overlay1 = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg_overlay1->AddEntry(h_e_f1, "Electrons", "l"); // 'l' for line style
    leg_overlay1->AddEntry(h_p_f1, "Protons", "l");
    leg_overlay1->AddEntry(h_g_f1, "Gammas", "l");
    leg_overlay1->AddEntry(h_pos_f1, "Positrons", "l");
    leg_overlay1->Draw();

    // --- Overlaid Plot for File 2 (With Shielding) ---
    c->cd(2);
    // Create histograms for each secondary particle type for File 2
    TH1F* h_e_f2 = new TH1F("h_e_f2", "Electrons (With Shielding);Plane ID;Counts", 9, -0.5, 8.5);
    TH1F* h_p_f2 = new TH1F("h_p_f2", "Protons (With Shielding);Plane ID;Counts", 9, -0.5, 8.5);
    TH1F* h_g_f2 = new TH1F("h_g_f2", "Gammas (With Shielding);Plane ID;Counts", 9, -0.5, 8.5);
    TH1F* h_pos_f2 = new TH1F("h_pos_f2", "Positrons (With Shielding);Plane ID;Counts", 9, -0.5, 8.5);

    // Set line colors and styles for overlaying
    h_e_f2->SetLineColor(kRed);     h_e_f2->SetLineStyle(1); h_e_f2->SetLineWidth(2);
    h_p_f2->SetLineColor(kGreen+2);  h_p_f2->SetLineStyle(2); h_p_f2->SetLineWidth(2);
    h_g_f2->SetLineColor(kBlue);    h_g_f2->SetLineStyle(3); h_g_f2->SetLineWidth(2);
    h_pos_f2->SetLineColor(kOrange+7); h_pos_f2->SetLineStyle(4); h_pos_f2->SetLineWidth(2);

    // Loop through planes to fill the particle type histograms for File 2
    for (int plane = 0; plane < 9; ++plane) {
        TString cut_e = Form("planeID==%d && trackID!=1 && pdgID==11", plane);
        TString cut_p = Form("planeID==%d && trackID!=1 && pdgID==2212", plane);
        TString cut_g = Form("planeID==%d && trackID!=1 && pdgID==22", plane);
        TString cut_pos = Form("planeID==%d && trackID!=1 && pdgID==-11", plane);

        h_e_f2->SetBinContent(plane + 1, t2->GetEntries(cut_e));
        h_p_f2->SetBinContent(plane + 1, t2->GetEntries(cut_p));
        h_g_f2->SetBinContent(plane + 1, t2->GetEntries(cut_g));
        h_pos_f2->SetBinContent(plane + 1, t2->GetEntries(cut_pos));
    }

    // Determine the maximum Y value across all overlaid histograms for proper scaling
    double max_overlay_f2 = TMath::Max(h_e_f2->GetMaximum(), h_p_f2->GetMaximum());
    max_overlay_f2 = TMath::Max(max_overlay_f2, h_g_f2->GetMaximum());
    max_overlay_f2 = TMath::Max(max_overlay_f2, h_pos_f2->GetMaximum());

    // Draw the first histogram, then overlay others
    h_e_f2->GetYaxis()->SetRangeUser(0, max_overlay_f2 * 1.2); // Set Y-axis range
    h_e_f2->SetTitle("Secondary Particle Composition (With Shielding);Plane ID;Counts");
    h_e_f2->Draw("HIST");
    h_p_f2->Draw("HIST SAME");
    h_g_f2->Draw("HIST SAME");
    h_pos_f2->Draw("HIST SAME");

    TLegend* leg_overlay2 = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg_overlay2->AddEntry(h_e_f2, "Electrons", "l");
    leg_overlay2->AddEntry(h_p_f2, "Protons", "l");
    leg_overlay2->AddEntry(h_g_f2, "Gammas", "l");
    leg_overlay2->AddEntry(h_pos_f2, "Positrons", "l");
    leg_overlay2->Draw();

    c->Print(pdfName); // Print the page with overlaid plots to PDF

    // Clean up overlaid plot histograms and legends
    delete h_e_f1; delete h_p_f1; delete h_g_f1; delete h_pos_f1;
    delete h_e_f2; delete h_p_f2; delete h_g_f2; delete h_pos_f2;
    delete leg_overlay1; delete leg_overlay2;

    c->Print(pdfName + "]"); // Close the PDF file

    // Clean up remaining objects created outside the loop
    delete hPrim1;
    delete hPrim2;
    delete hSec1;
    delete hSec2;
    delete legPrim;
    delete legSec;

    delete c; // Delete the canvas
    f1->Close(); // Close ROOT files
    f2->Close();
    delete f1; // Delete file pointers
    delete f2;
}

