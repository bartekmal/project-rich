#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

void DrawOccupancyRatio(const std::string dir1, const std::string dir2)
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
  TFile *f = TFile::Open(TString(dir1 + "/Gauss-Histo.root"));
  TFile *f2 = TFile::Open(TString(dir2 + "/Boole-Histo.root"));

  TFile *fOut = TFile::Open("occupancyRatio_GaussToBoole.root", "new");

  const TString digitQCPath = "RICH/DIGI/DIGITQC/";

  //main
  TH1D *R1OverallNH = (TH1D *)f->Get("RICHG4HISTOSET5/363");
  TH2D *R1XY = (TH2D *)f->Get("RICHG4HISTOSET5/360");
  TH1D *R1PmtOccup = (TH1D *)f->Get("RICHG4HISTOSET5/365");
  //R1PmtOccup->Rebin(2);
  TH1D *R1PmtOccupPercent = (TH1D *)R1PmtOccup->Clone("R1PmtOccupPerCent");

  TH1D *R2OverallNH = (TH1D *)f->Get("RICHG4HISTOSET5/383");
  TH2D *R2XY = (TH2D *)f->Get("RICHG4HISTOSET5/380");
  TH1D *R2PmtOccup = (TH1D *)f->Get("RICHG4HISTOSET5/385");
  //R2PmtOccup->Rebin(2);
  TH1D *R2PmtOccupPercent = (TH1D *)R2PmtOccup->Clone("R2PmtOccupPerCent");

  //ref
  TH1D *R1OverallNH_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich1 : Detector occupancy"));
  TH2D *R1XY_ref = (TH2D *)f2->Get(TString(digitQCPath + "Rich1 : Hit map (direct from digits - global)"));
  TH1D *R1PmtOccup_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich1 : PMT occupancy (nHits>0)"));
  TH1D *R1PmtOccupPercent_ref = (TH1D *)R1PmtOccup_ref->Clone("R1PmtOccupPerCent");

  TH1D *R2OverallNH_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich2 : Detector occupancy"));
  TH2D *R2XY_ref = (TH2D *)f2->Get(TString(digitQCPath + "Rich2 : Hit map (direct from digits - global)"));
  TH1D *R2PmtOccup_ref = (TH1D *)f2->Get(TString(digitQCPath + "Rich2 : PMT occupancy (nHits>0)"));
  TH1D *R2PmtOccupPercent_ref = (TH1D *)R2PmtOccup_ref->Clone("R2PmtOccupPerCent");

  //ratios
  TH1D *R1XYRatio = (TH1D *)R1XY_ref->Clone("R1XYRatio");
  TH1D *R2XYRatio = (TH1D *)R2XY_ref->Clone("R2XYRatio");
  TH1D *R1PmtOccupRatio = (TH1D *)R1PmtOccupPercent->Clone("R1PmtOccupRatio");
  TH1D *R2PmtOccupRatio = (TH1D *)R2PmtOccupPercent->Clone("R2PmtOccupRatio");
  /*
  //fix errors...   
  for(Int_t ii = 0; ii < R1XYRatio->GetNbinsX(); ++ii) {
    for(Int_t jj = 0; jj < R1XYRatio->GetNbinsY(); ++jj) {
      R1XYRatio->SetBinError(ii+1,jj+1,1000.0);
    }
  }
  for(Int_t ii = 0; ii < R2XYRatio->GetNbinsX(); ++ii) {
    for(Int_t jj = 0; jj < R2XYRatio->GetNbinsY(); ++jj) {
      R2XYRatio->SetBinError(ii+1,jj+1,1000.0);
    }
  }
  for(Int_t ii = 0; ii < R1PmtOccupRatio->GetNbinsX(); ++ii) R1PmtOccupRatio->SetBinError(ii+1,1000.0);
  for(Int_t ii = 0; ii < R2PmtOccupRatio->GetNbinsX(); ++ii) R2PmtOccupRatio->SetBinError(ii+1,1000.0);
  */

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
  R1PmtOccupRatio->Divide(R1PmtOccupPercent, R1PmtOccupPercent_ref);
  R2PmtOccupRatio->Divide(R2PmtOccupPercent, R2PmtOccupPercent_ref);

  //set ranges
  R1XYRatio->GetZaxis()->SetRangeUser(0, 3);
  R1PmtOccupRatio->GetYaxis()->SetRangeUser(0.9, R1PmtOccupRatio->GetMaximum() * 1.1);
  R2PmtOccupRatio->GetYaxis()->SetRangeUser(0.9, R2PmtOccupRatio->GetMaximum() * 1.1);

  //save plots
  TCanvas *c1 = new TCanvas("c1", "Rich1 Pmt Occupancy ratio (with/no SIN)", canvasSizeX, canvasSizeY);
  R1PmtOccupRatio->Draw();
  TCanvas *c2 = new TCanvas("c2", "Rich1 Pmt XY Occupancy ratio (with/no SIN)", canvasSizeX, canvasSizeY);
  c2->SetRightMargin(rightMargin);
  R1XYRatio->Draw("colz");

  c1->SaveAs("R1_occupancy_ratio_1D.pdf");
  //c2 -> SaveAs("R1_occupancy_ratio_2D.pdf");

  TCanvas *c1_2 = new TCanvas("c1_2", "Rich2 Pmt Occupancy ratio (with/no SIN)", canvasSizeX, canvasSizeY);
  R2PmtOccupRatio->Draw();
  TCanvas *c2_2 = new TCanvas("c2_2", "Rich2 Pmt XY Occupancy ratio (with/no SIN)", canvasSizeX, canvasSizeY);
  c2_2->SetRightMargin(rightMargin);
  R2XYRatio->Draw("colz");

  c1_2->SaveAs("R2_occupancy_ratio_1D.pdf");
  //c2_2 -> SaveAs("R2_occupancy_ratio_2D.pdf");

  fOut->Write();

  f->Close();
  f2->Close();
  fOut->Close();
}
