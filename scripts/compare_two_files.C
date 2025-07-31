#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TString.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TPad.h"

#include <iostream>

void compare_two_files(const char* file1 = "../build/Pro_6GeV_0T_0deg_Honeycomb_NoTarget.root", const char* file2 = "../build/Pro_6GeV_0T_0deg_FR4_NoTarget.root") {
    gROOT->SetBatch(kTRUE); // Run in batch mode, no pop-up canvases
    gStyle->SetOptStat(0);   // Do not show statistics box on plots by default

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
        f1->Close();
        f2->Close();
        return;
    }
    //------Legends Name-----
    TString Leg1 = "Pro: HC : 6GeV, FR4"; 
    TString Leg2 = "Pro: FR4"; 

    TString pdfName = "comparison.pdf";
    TString pdfStart = pdfName + "[";
    TString pdfEnd = pdfName + "]";

    TCanvas* c = new TCanvas("c", "Comparison", 1200, 800);
    c->Print(pdfStart);

    // --- Primary and Secondary Hits per Plane ---
    TH1F* hPrim1 = new TH1F("hPrim1", "Primary Hits per Plane;Plane ID;Counts", 11, -0.5, 10.5);
    TH1F* hPrim2 = new TH1F("hPrim2", "", 10, -0.5, 9.5); // Title will be set by legend

    TH1F* hSec1  = new TH1F("hSec1", "Secondary Hits per Plane;Plane ID;Counts", 11, -0.5, 10.5);
    TH1F* hSec2  = new TH1F("hSec2", "", 10, -0.5, 9.5); // Title will be set by legend

    for (int plane = 0; plane < 11; ++plane) {
        TString cut1 = Form("planeID==%d && trackID==1 && parentID == 0", plane);
        TString cut2 = Form("planeID==%d && trackID!=1 && pdgID==11 && parentID == 1", plane);
        //TString cut2 = Form("planeID==%d && trackID!=1 && (pdgID==211 || pdgID== -211 || pdgID == 111 pdgID == 2112)", plane);
        hPrim1->SetBinContent(plane + 1, t1->GetEntries(cut1)); // +1 because bin indexing starts from 1
        hPrim2->SetBinContent(plane + 1, t2->GetEntries(cut1));
        hSec1->SetBinContent(plane + 1, t1->GetEntries(cut2));
        hSec2->SetBinContent(plane + 1, t2->GetEntries(cut2));
    }

    // Set styles for Primary Hits per Plane
    hPrim1->SetLineColor(kBlue);
    hPrim1->SetFillColorAlpha(kBlue, 0.3);
    hPrim1->GetYaxis()->SetTitle("Counts");
    hPrim1->GetXaxis()->SetTitle("Plane ID");

    hPrim2->SetLineColor(kRed);
    hPrim2->SetLineStyle(2);
    hPrim2->SetMarkerStyle(20);
    hPrim2->GetYaxis()->SetTitle("Counts");
    hPrim2->GetXaxis()->SetTitle("Plane ID");


    // Set styles for Secondary Hits per Plane
    hSec1->SetLineColor(kBlue);
    hSec1->SetFillColorAlpha(kBlue, 0.3);
    hSec1->GetYaxis()->SetTitle("Counts");
    hSec1->GetXaxis()->SetTitle("Plane ID");

    hSec2->SetLineColor(kRed);
    hSec2->SetLineStyle(2);
    hSec2->SetMarkerStyle(20);
    hSec2->GetYaxis()->SetTitle("Counts");
    hSec2->GetXaxis()->SetTitle("Plane ID");

    // Adjust Y-axis range for hit count plots
    double maxPrim = TMath::Max(hPrim1->GetMaximum(), hPrim2->GetMaximum());
    hPrim1->GetYaxis()->SetRangeUser(0, maxPrim * 1.1);
    hPrim2->GetYaxis()->SetRangeUser(0, maxPrim * 1.1);

    double maxSec = TMath::Max(hSec1->GetMaximum(), hSec2->GetMaximum());
    hSec1->GetYaxis()->SetRangeUser(0, maxSec * 1.1);
    hSec2->GetYaxis()->SetRangeUser(0, maxSec * 1.1);


    c->Clear();
    c->Divide(1,2); // Divide canvas for primary and secondary hit counts

    c->cd(1);
    hPrim1->Draw("HIST");
    hPrim2->Draw("P SAME");
    TLegend* legPrim = new TLegend(0.15, 0.75, 0.45, 0.88); // Adjusted legend position
    legPrim->AddEntry(hPrim1, "FR4 :6GeV", "lf"); // 'lf' for line and fill style
    legPrim->AddEntry(hPrim2, "HoneyComb :6GeV", "lp"); // 'lp' for line and point style
    legPrim->Draw();
    hPrim1->SetTitle("Primary Hits per Plane"); // Set title explicitly after drawing

    c->cd(2);
    hSec1->Draw("HIST");
    hSec2->Draw("P SAME");
    TLegend* legSec = new TLegend(0.15, 0.75, 0.45, 0.88); // Adjusted legend position
    legSec->AddEntry(hSec1, Leg1, "lf");
    legSec->AddEntry(hSec2, Leg2, "lp");
    legSec->Draw();
    hSec1->SetTitle("Secondary Hits per Plane"); // Set title explicitly after drawing
    c->Print(pdfName);

    // Loop through each plane for detailed plots
/*    for (int plane = 0; plane < 10; ++plane) {
        TString cut1 = Form("planeID==%d && trackID==1", plane);
        TString cut2 = Form("planeID==%d && trackID!=1", plane);

        // --- Energy Histograms ---
        c->Clear();
        c->Divide(2,1);

        // Primary Energy
        c->cd(1);
        TH1F* h1_energy = new TH1F(Form("h1_energy_plane%d", plane), Form("Primary Energy (Plane %d);Energy (MeV);Counts", plane), 100, 1000, 12010);
        t1->Draw(Form("energy >> %s", h1_energy->GetName()), cut1, "HIST"); // Draw to the named histogram
        h1_energy->SetLineColor(kBlue);
        h1_energy->GetXaxis()->SetTitle("Energy (MeV)");
        h1_energy->GetYaxis()->SetTitle("Counts");

        TH1F* h2_energy = new TH1F(Form("h2_energy_plane%d", plane), "", 100, 1000, 12010);
        t2->Draw(Form("energy >> %s", h2_energy->GetName()), cut1, "HIST SAME"); // Draw to the named histogram, same pad
        h2_energy->SetLineColor(kRed);

        TLegend* leg_prim_energy = new TLegend(0.15, 0.75, 0.45, 0.88); // Adjusted legend position
        leg_prim_energy->AddEntry(h1_energy, Leg1, "l");
        leg_prim_energy->AddEntry(h2_energy, Leg2, "l");
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

        TLegend* leg_sec_energy = new TLegend(0.15, 0.75, 0.45, 0.88); // Adjusted legend position
        leg_sec_energy->AddEntry(hs1_energy, Leg1, "l");
        leg_sec_energy->AddEntry(hs2_energy, Leg2, "l");
        leg_sec_energy->Draw();

        c->Print(pdfName);

        // --- 1D X hits ---
        c->Clear();
        c->Divide(2,1);

        // Primary X
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
        leg_prim_x->AddEntry(hx1, Leg1, "l");
        leg_prim_x->AddEntry(hx2, Leg2, "l");
        leg_prim_x->Draw();

        // Secondary X
        c->cd(2);
        TH1F* hxs1 = new TH1F(Form("hxs1_plane%d", plane), Form("X (Secondary) Plane %d;X [mm];Counts", plane), 120, -60, 60);
        t1->Draw(Form("x >> %s", hxs1->GetName()), cut2, "HIST");
        hxs1->SetLineColor(kBlue);
        hxs1->GetXaxis()->SetTitle("X [mm]");
        hxs1->GetYaxis()->SetTitle("Counts");
        //hxs1->GetYaxis()->SetRangeUser(0, 150);

        TH1F* hxs2 = new TH1F(Form("hxs2_plane%d", plane), "", 120, -60, 60);
        t2->Draw(Form("x >> %s", hxs2->GetName()), cut2, "HIST SAME");
        hxs2->SetLineColor(kRed);
        TLegend* leg_sec_x = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_sec_x->AddEntry(hxs1, Leg1, "l");
        leg_sec_x->AddEntry(hxs2, Leg2, "l");
        leg_sec_x->Draw();

        c->Print(pdfName);

        // --- Theta and Phi ---
        c->Clear();
        c->Divide(2,2); // Divide for 4 plots: Primary Theta, Primary Phi, Secondary Theta, Secondary Phi

        // Primary Theta
        c->cd(1);
        TH1F* th1 = new TH1F(Form("th1_plane%d", plane), Form("#theta (Primary) Plane %d;#theta [deg];Counts", plane), 100, 0, 30.0);
        t1->Draw(Form("theta*180/TMath::Pi() >> %s", th1->GetName()), cut1, "HIST");
        th1->SetLineColor(kBlue);
        th1->GetXaxis()->SetTitle("#theta [deg]");
        th1->GetYaxis()->SetTitle("Counts");

        TH1F* th2 = new TH1F(Form("th2_plane%d", plane), "", 100, 0, 10.0);
        t2->Draw(Form("theta*180/TMath::Pi() >> %s", th2->GetName()), cut1, "HIST SAME");
        th2->SetLineColor(kRed);
        TLegend* leg_prim_theta = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_prim_theta->AddEntry(th1, Leg1, "l");
        leg_prim_theta->AddEntry(th2, Leg2, "l");
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
        leg_prim_phi->AddEntry(ph1, Leg1, "l");
        leg_prim_phi->AddEntry(ph2, Leg2, "l");
        leg_prim_phi->Draw();

        // Secondary Theta
        c->cd(3); // Changed to cd(3)
        TH1F* ths1 = new TH1F(Form("ths1_plane%d", plane), Form("#theta (Secondary) Plane %d;#theta [deg];Counts", plane), 100, 0, 90.0);
        t1->Draw(Form("theta*180/TMath::Pi() >> %s", ths1->GetName()), cut2, "HIST");
        ths1->SetLineColor(kBlue);
        ths1->GetXaxis()->SetTitle("#theta [deg]");
        ths1->GetYaxis()->SetTitle("Counts");
        //ths1->GetYaxis()->SetRangeUser(0, 100);

        TH1F* ths2 = new TH1F(Form("ths2_plane%d", plane), "", 100, 0, 10.0);
        t2->Draw(Form("theta*180/TMath::Pi() >> %s", ths2->GetName()), cut2, "HIST SAME");
        ths2->SetLineColor(kRed);
        TLegend* leg_sec_theta = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_sec_theta->AddEntry(ths1, Leg1, "l");
        leg_sec_theta->AddEntry(ths2, Leg2, "l");
        leg_sec_theta->Draw();

        // Secondary Phi
        c->cd(4); // Changed to cd(4)
        TH1F* phs1 = new TH1F(Form("phs1_plane%d", plane), Form("#phi (Secondary) Plane %d;#phi [deg];Counts", plane), 100, -180, 180);
        t1->Draw(Form("phi*180/TMath::Pi() >> %s", phs1->GetName()), cut2, "HIST");
        phs1->SetLineColor(kBlue);
        phs1->GetXaxis()->SetTitle("#phi [deg]");
        phs1->GetYaxis()->SetTitle("Counts");
        //phs1->GetYaxis()->SetRangeUser(0, 100);

        TH1F* phs2 = new TH1F(Form("phs2_plane%d", plane), "", 100, -180, 180);
        t2->Draw(Form("phi*180/TMath::Pi() >> %s", phs2->GetName()), cut2, "HIST SAME");
        phs2->SetLineColor(kRed);
        TLegend* leg_sec_phi = new TLegend(0.15, 0.75, 0.45, 0.88);
        leg_sec_phi->AddEntry(phs1, Leg1, "l");
        leg_sec_phi->AddEntry(phs2, Leg2, "l");
        leg_sec_phi->Draw();

        c->Print(pdfName);

        // Delete histograms to avoid memory leaks for each plane
        delete h1_energy; delete h2_energy;
        delete hs1_energy; delete hs2_energy;
        delete hx1; delete hx2;
        delete hxs1; delete hxs2;
        delete th1; delete th2;
        delete ph1; delete ph2;
        delete ths1; delete ths2;
        delete phs1; delete phs2;

        delete leg_prim_energy; delete leg_sec_energy;
        delete leg_prim_x; delete leg_sec_x;
        delete leg_prim_theta; delete leg_prim_phi;
        delete leg_sec_theta; delete leg_sec_phi;
    }
*/
    c->Print(pdfName + "]"); // Close the PDF file

    // Clean up
    delete hPrim1;
    delete hPrim2;
    delete hSec1;
    delete hSec2;
    delete legPrim;
    delete legSec;

    delete c; // Delete the canvas
    f1->Close();
    f2->Close();
    delete f1;
    delete f2;
}
