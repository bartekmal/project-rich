#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

void DrawOccupancyRatio(const std::string dir1, const std::string dir2, const std::string dir3 = "")
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
  gStyle->SetMarkerColor(kRed);
  gROOT->ForceStyle();

  //load histograms
  TFile *f = TFile::Open(TString(dir1 + "/Boole-Histo.root"));
  TFile *f2 = TFile::Open(TString(dir2 + "/Boole-Histo.root"));

  const TString digitQCPath = "RICH/DIGI/DIGITQC/";

  //main
  TH1D *R1OverallNH = (TH1D *)f->Get(TString(digitQCPath + "Rich1 : Detector occupancy"));
  TH2D *R1XY = (TH2D *)f->Get(TString(digitQCPath + "Rich1 : Tot. channel occupancy XY map"));
  TH1D *R1PmtOccup = (TH1D *)f->Get(TString(digitQCPath + "Rich1 : Tot. occupancy per PMT_ID"));
  TH1D *R1PmtOccupPercent = (TH1D *)R1PmtOccup->Clone("Rich1 : Av. channel occupancy per PMT_ID");
  R1XY->SetTitle("Rich1 : Av. channel occupancy XY map");

  TH1D *R2OverallNH = (TH1D *)f->Get(TString(digitQCPath + "Rich2 : Detector occupancy"));
  TH2D *R2XY = (TH2D *)f->Get(TString(digitQCPath + "Rich2 : Tot. channel occupancy XY map"));
  TH1D *R2PmtOccup = (TH1D *)f->Get(TString(digitQCPath + "Rich2 : Tot. occupancy per PMT_ID"));
  TH1D *R2PmtOccupPercent = (TH1D *)R2PmtOccup->Clone("Rich2 : Av. channel occupancy per PMT_ID");
  R2XY->SetTitle("Rich2 : Av. channel occupancy XY map");

  //ref
  TH1D *R1OverallNH_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich1 : Detector occupancy"));
  TH2D *R1XY_ref = (TH2D *)f2->Get(TString(digitQCPath + "Rich1 : Tot. channel occupancy XY map"));
  TH1D *R1PmtOccup_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich1 : Tot. occupancy per PMT_ID"));
  TH1D *R1PmtOccupPercent_ref = (TH1D *)R1PmtOccup_ref->Clone("Rich1 : Av. channel occupancy per PMT_ID");
  R1XY_ref->SetTitle("Rich1 : Av. channel occupancy XY map");

  TH1D *R2OverallNH_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich2 : Detector occupancy"));
  TH2D *R2XY_ref = (TH2D *)f2->Get(TString(digitQCPath + "Rich2 : Tot. channel occupancy XY map"));
  TH1D *R2PmtOccup_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich2 : Tot. occupancy per PMT_ID"));
  TH1D *R2PmtOccupPercent_ref = (TH1D *)R2PmtOccup_ref->Clone("Rich2 : Av. channel occupancy per PMT_ID");
  R2XY_ref->SetTitle("Rich2 : Av. channel occupancy XY map");

  //ratios
  TH1D *R1XYRatio = (TH1D *)R1XY_ref->Clone("R1XYRatio");
  TH1D *R2XYRatio = (TH1D *)R2XY_ref->Clone("R2XYRatio");
  TH1D *R1PmtOccupRatio = (TH1D *)R1PmtOccupPercent->Clone("R1PmtOccupRatio");
  TH1D *R2PmtOccupRatio = (TH1D *)R2PmtOccupPercent->Clone("R2PmtOccupRatio");

  R1XYRatio->SetTitle("Rich1 : Av. channel occupancy XY map ( ratio with/no SIN )");
  R2XYRatio->SetTitle("Rich2 : Av. channel occupancy XY map ( ratio with/no SIN )");
  R1PmtOccupRatio->SetTitle("Rich1 : Av. channel occupancy per PMT_ID ( ratio with/no SIN )");
  R2PmtOccupRatio->SetTitle("Rich2 : Av. channel occupancy per PMT_ID ( ratio with/no SIN )");

  // deal with errors
  R1XY->Sumw2();
  R1PmtOccup->Sumw2();
  R1PmtOccupPercent->Sumw2();
  R2XY->Sumw2();
  R2PmtOccup->Sumw2();
  R2PmtOccupPercent->Sumw2();
  R1XY_ref->Sumw2();
  R1PmtOccup_ref->Sumw2();
  R1PmtOccupPercent_ref->Sumw2();
  R2XY_ref->Sumw2();
  R2PmtOccup_ref->Sumw2();
  R2PmtOccupPercent_ref->Sumw2();

  //normalize
  //main
  Int_t numEvR1 = R1OverallNH->GetEntries();
  std::cout << " Number of events Rich1= " << numEvR1 << std::endl;
  R1XY->Scale(1.0 / (numEvR1 * 1.0));
  R1PmtOccup->Scale(1.0 / (numEvR1 * 1.0));
  R1PmtOccupPercent->Scale(1.0 / (numEvR1 * 1.0) * 100.0 / 64.0);

  Int_t numEvR2 = R2OverallNH->GetEntries();
  std::cout << " Number of events Rich2= " << numEvR2 << std::endl;
  R2XY->Scale(1.0 / (numEvR2 * 1.0));
  R2PmtOccup->Scale(1.0 / (numEvR2 * 1.0));
  R2PmtOccupPercent->Scale(1.0 / (numEvR2 * 1.0) * 100.0 / 64.0);

  //ref
  Int_t numEvR1_ref = R1OverallNH_ref->GetEntries();
  std::cout << " Number of events Rich1= " << numEvR1_ref << std::endl;
  R1XY_ref->Scale(1.0 / (numEvR1_ref * 1.0));
  R1PmtOccup_ref->Scale(1.0 / (numEvR1_ref * 1.0));
  R1PmtOccupPercent_ref->Scale(1.0 / (numEvR1_ref * 1.0) * 100.0 / 64.0);

  Int_t numEvR2_ref = R2OverallNH_ref->GetEntries();
  std::cout << " Number of events Rich2= " << numEvR2_ref << std::endl;
  R2XY_ref->Scale(1.0 / (numEvR2_ref * 1.0));
  R2PmtOccup_ref->Scale(1.0 / (numEvR2_ref * 1.0));
  R2PmtOccupPercent_ref->Scale(1.0 / (numEvR2_ref * 1.0) * 100.0 / 64.0);

  //ratios
  R1XYRatio->Divide(R1XY, R1XY_ref);
  R2XYRatio->Divide(R2XY, R2XY_ref);
  const auto normR1PmtOccupPercent = R1PmtOccupPercent->GetEntries();
  const auto normR1PmtOccupPercent_ref = R1PmtOccupPercent_ref->GetEntries();
  const auto normR2PmtOccupPercent = R2PmtOccupPercent->GetEntries();
  const auto normR2PmtOccupPercent_ref = R2PmtOccupPercent_ref->GetEntries();
  R1PmtOccupRatio->Divide(R1PmtOccupPercent, R1PmtOccupPercent_ref);
  R2PmtOccupRatio->Divide(R2PmtOccupPercent, R2PmtOccupPercent_ref);
  //R1PmtOccupRatio->Divide( R1PmtOccupPercent, R1PmtOccupPercent_ref, normR1PmtOccupPercent_ref, normR1PmtOccupPercent );
  //R2PmtOccupRatio->Divide( R2PmtOccupPercent, R2PmtOccupPercent_ref, normR2PmtOccupPercent_ref, normR2PmtOccupPercent );
  /*
  //correct 1D occupancy for Gauss/Boole occupancy ratio
  {
    TFile* f_ratios = TFile::Open(TString(dir3+"/occupancyRatio_GaussToBoole.root"));  
  
    TH1D* correctForOccR1 = (TH1D*) f_ratios->Get( "R1PmtOccupRatio" );
    TH1D* correctForOccR2 = (TH1D*) f_ratios->Get( "R2PmtOccupRatio" );
    
    R1PmtOccupRatio->Divide( R1PmtOccupRatio, correctForOccR1 );
    R2PmtOccupRatio->Divide( R2PmtOccupRatio, correctForOccR2 );

    f_ratios->Close();
  }
  */
  //set ranges
  const Double_t ratioRangeMin = 0.8; // (0 - 3; 0.95 - 2.2)
  const Double_t ratioRangeMax = 1.5;
  R1XYRatio->GetZaxis()->SetRangeUser(ratioRangeMin, ratioRangeMax);
  R2XYRatio->GetZaxis()->SetRangeUser(ratioRangeMin, ratioRangeMax);
  //const auto maxOccupancyPlot_R1 = R1PmtOccupRatio->GetMaximum() * 1.05;
  //R1PmtOccupRatio->GetYaxis()->SetRangeUser(0.95,maxOccupancyPlot_R1);
  //R2PmtOccupRatio->GetYaxis()->SetRangeUser(0.95,maxOccupancyPlot_R1);
  R1PmtOccupRatio->GetYaxis()->SetRangeUser(ratioRangeMin, ratioRangeMax);
  R2PmtOccupRatio->GetYaxis()->SetRangeUser(ratioRangeMin, ratioRangeMax);

  //save plots
  TCanvas *c1 = new TCanvas("c1", "Rich1 : Av. channel occupancy per PMT_ID ( ratio with/no SIN )", canvasSizeX, canvasSizeY);
  R1PmtOccupRatio->Draw();
  TCanvas *c2 = new TCanvas("c2", "Rich1 : Av. channel occupancy XY map ( ratio with/no SIN )", canvasSizeX, canvasSizeY);
  c2->SetRightMargin(rightMargin);
  R1XYRatio->Draw("colz");

  c1->SaveAs("occupancy_Boole_R1_ratio.pdf");
  c2->SaveAs("occupancyMap_Boole_R1_ratio.pdf");

  TCanvas *c1_2 = new TCanvas("c1_2", "Rich2 : Av. channel occupancy per PMT_ID ( ratio with/no SIN )", canvasSizeX, canvasSizeY);
  R2PmtOccupRatio->Draw();
  TCanvas *c2_2 = new TCanvas("c2_2", "Rich2 : Av. channel occupancy XY map ( ratio with/no SIN )", canvasSizeX, canvasSizeY);
  c2_2->SetRightMargin(rightMargin);
  R2XYRatio->Draw("colz");

  c1_2->SaveAs("occupancy_Boole_R2_ratio.pdf");
  c2_2->SaveAs("occupancyMap_Boole_R2_ratio.pdf");

  f->Close();
  f2->Close();
}
