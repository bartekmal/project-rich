#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

void DrawSinOccupancyProfile(const std::string dir1, const std::string dir2, const std::string dir3 = "")
{

    //config style
    Int_t canvasSizeX = 1200;
    Int_t canvasSizeY = 1540;
    Double_t rightMargin = 0.125;
    //  gStyle->SetOptStat(0010);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(104);
    // gStyle->SetPalette(kNeon);
    //   gStyle->SetPalette(kRainBow);
    // gStyle->SetPalette(kVisibleSpectrum);

    //load histograms
    TFile *f = TFile::Open(TString(dir1 + "/Boole-Histo.root"));
    TFile *f2 = TFile::Open(TString(dir2 + "/Boole-Histo.root"));

    const TString digitQCPath = "RICH/DIGI/DIGITQC/";

    //main
    TH1D *R1OverallNH = (TH1D *)f->Get(TString(digitQCPath + "Rich1 : Detector occupancy"));
    TH1D *R1PmtOccup = (TH1D *)f->Get(TString(digitQCPath + "Rich1 : Tot. occupancy per PMT_ID"));
    TH1D *R1PmtOccupPercent = (TH1D *)R1PmtOccup->Clone("Rich1 : Av. channel occupancy per PMT_ID");

    TH1D *R2OverallNH = (TH1D *)f->Get(TString(digitQCPath + "Rich2 : Detector occupancy"));
    TH1D *R2PmtOccup = (TH1D *)f->Get(TString(digitQCPath + "Rich2 : Tot. occupancy per PMT_ID"));
    TH1D *R2PmtOccupPercent = (TH1D *)R2PmtOccup->Clone("Rich2 : Av. channel occupancy per PMT_ID");

    //ref
    TH1D *R1OverallNH_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich1 : Detector occupancy"));
    TH1D *R1PmtOccup_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich1 : Tot. occupancy per PMT_ID"));
    TH1D *R1PmtOccupPercent_ref = (TH1D *)R1PmtOccup_ref->Clone("Rich1 : Av. channel occupancy per PMT_ID");

    TH1D *R2OverallNH_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich2 : Detector occupancy"));
    TH1D *R2PmtOccup_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich2 : Tot. occupancy per PMT_ID"));
    TH1D *R2PmtOccupPercent_ref = (TH1D *)R2PmtOccup_ref->Clone("Rich2 : Av. channel occupancy per PMT_ID");

    //profiles
    TH2D *R1PmtOccupProfile = new TH2D("R1PmtOccupProfile", "", 100, 0., 50., 100, 0., 100.);
    TH2D *R2PmtOccupProfile = new TH2D("R1PmtOccupProfile", "", 100, 0., 50., 100, 0., 100.);
    R1PmtOccupProfile->SetTitle("Rich1 : Av. channel occupancy ( with VS without SIN )");
    R2PmtOccupProfile->SetTitle("Rich2 : Av. channel occupancy ( with VS without SIN )");

    //normalize
    //main
    Int_t numEvR1 = R1OverallNH->GetEntries();
    std::cout << " Number of events Rich1= " << numEvR1 << std::endl;
    R1PmtOccup->Scale(1.0 / (numEvR1 * 1.0));
    R1PmtOccupPercent->Scale(1.0 / (numEvR1 * 1.0) * 100.0 / 64.0);

    Int_t numEvR2 = R2OverallNH->GetEntries();
    std::cout << " Number of events Rich2= " << numEvR2 << std::endl;
    R2PmtOccup->Scale(1.0 / (numEvR2 * 1.0));
    R2PmtOccupPercent->Scale(1.0 / (numEvR2 * 1.0) * 100.0 / 64.0);

    //ref
    Int_t numEvR1_ref = R1OverallNH_ref->GetEntries();
    std::cout << " Number of events Rich1= " << numEvR1_ref << std::endl;
    R1PmtOccup_ref->Scale(1.0 / (numEvR1_ref * 1.0));
    R1PmtOccupPercent_ref->Scale(1.0 / (numEvR1_ref * 1.0) * 100.0 / 64.0);

    Int_t numEvR2_ref = R2OverallNH_ref->GetEntries();
    std::cout << " Number of events Rich2= " << numEvR2_ref << std::endl;
    R2PmtOccup_ref->Scale(1.0 / (numEvR2_ref * 1.0));
    R2PmtOccupPercent_ref->Scale(1.0 / (numEvR2_ref * 1.0) * 100.0 / 64.0);

    //profiles
    const auto nrBinsHistR1 = R1PmtOccupPercent->GetNbinsX();
    const auto nrBinsHistR2 = R2PmtOccupPercent->GetNbinsX();

    for (int i = 0; i < nrBinsHistR1; ++i)
    {
        // take only non-empty bins
        if (R1PmtOccupPercent_ref->GetBinContent(i) > 0)
            R1PmtOccupProfile->Fill(R1PmtOccupPercent_ref->GetBinContent(i), R1PmtOccupPercent->GetBinContent(i));
    }

    for (int i = 0; i < nrBinsHistR2; ++i)
    {
        // take only non-empty bins
        if (R2PmtOccupPercent_ref->GetBinContent(i) > 0)
            R2PmtOccupProfile->Fill(R2PmtOccupPercent_ref->GetBinContent(i), R2PmtOccupPercent->GetBinContent(i));
    }

    //save plots
    TCanvas *c1 = new TCanvas("c1", "Rich1 : Av. channel occupancy ( with VS without SIN )", canvasSizeX, canvasSizeY);
    R1PmtOccupProfile->Draw("COLZ");

    c1->SaveAs("occupancy_Boole_R1_profileSin.pdf");

    TCanvas *c1_2 = new TCanvas("c1_2", "Rich2 : Av. channel occupancy ( with VS without SIN )", canvasSizeX, canvasSizeY);
    R2PmtOccupProfile->Draw("COLZ");

    c1_2->SaveAs("occupancy_Boole_R2_profileSin.pdf");

    f->Close();
    f2->Close();
}
