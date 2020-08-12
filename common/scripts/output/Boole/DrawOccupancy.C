#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

void DrawOccupancy(const std::string dir1)
{

  // config histograms
  const float occupancyYMaxR1 = 0.50;
  const float occupancyYMaxR2 = 0.20;

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

  const TString digitQCPath = "RICH/DIGI/DIGITQC/";

  TH1D *R1OverallNH = (TH1D *)f->Get(TString(digitQCPath + "Rich1 : Detector occupancy"));
  TH2D *R1XY = (TH2D *)f->Get(TString(digitQCPath + "Rich1 : Tot. channel occupancy XY map"));
  TH1D *R1PmtOccup = (TH1D *)f->Get(TString(digitQCPath + "Rich1 : Tot. occupancy per PMT_ID"));
  TH1D *R1PmtOccupPercent = (TH1D *)R1PmtOccup->Clone("R1PmtOccupPerCent");
  R1PmtOccupPercent->SetTitle("Rich1 : Av. channel occupancy per PMT_ID");
  R1XY->SetTitle("Rich1 : Av. channel occupancy XY map");

  TH1D *R2OverallNH = (TH1D *)f->Get(TString(digitQCPath + "Rich2 : Detector occupancy"));
  TH2D *R2XY = (TH2D *)f->Get(TString(digitQCPath + "Rich2 : Tot. channel occupancy XY map"));
  TH1D *R2PmtOccup = (TH1D *)f->Get(TString(digitQCPath + "Rich2 : Tot. occupancy per PMT_ID"));
  TH1D *R2PmtOccupPercent = (TH1D *)R2PmtOccup->Clone("R2PmtOccupPerCent");
  R2PmtOccupPercent->SetTitle("Rich2 : Av. channel occupancy per PMT_ID");
  R2XY->SetTitle("Rich2 : Av. channel occupancy XY map");

  //normalize
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

  //set axis ranges
  R1PmtOccupPercent->SetAxisRange(0, occupancyYMaxR1 * 100, "Y");
  R1XY->SetAxisRange(0., occupancyYMaxR1, "Z");
  R2PmtOccupPercent->SetAxisRange(0, occupancyYMaxR2 * 100, "Y");
  R2XY->SetAxisRange(0., occupancyYMaxR2, "Z");

  //save plots
  TCanvas *c1 = new TCanvas("c1", "Rich1 Pmt Occupancy", canvasSizeX, canvasSizeY);
  R1PmtOccup->Draw();
  TCanvas *c2 = new TCanvas("c2", "Rich1 XY Pmt ", canvasSizeX, canvasSizeY);
  c2->SetRightMargin(rightMargin);
  R1XY->Draw("colz");
  TCanvas *c1p = new TCanvas("c1p", "Rich1 Pmt Occupancy in Percent", canvasSizeX, canvasSizeY);
  R1PmtOccupPercent->SetTitle("RICH1 PMT Occupancy in Percent");
  R1PmtOccupPercent->Draw();

  //c1 -> SaveAs("totalOccupancy_Boole_R1.pdf");
  c2->SaveAs("occupancyMap_Boole_R1.pdf");
  c1p->SaveAs("occupancy_Boole_R1.pdf");
  // c1p -> SaveAs("occupancy_Boole_R1.C");

  TCanvas *c1_2 = new TCanvas("c1_2", "Rich2 Pmt Occupancy", canvasSizeX, canvasSizeY);
  R2PmtOccup->Draw();
  TCanvas *c2_2 = new TCanvas("c2_2", "Rich2 XY Pmt ", canvasSizeX, canvasSizeY);
  c2_2->SetRightMargin(rightMargin);
  R2XY->Draw("colz");
  TCanvas *c1p_2 = new TCanvas("c1p_2", "Rich2 Pmt Occupancy in Percent", canvasSizeX, canvasSizeY);
  R2PmtOccupPercent->SetTitle("RICH2 PMT Occupancy in Percent");
  R2PmtOccupPercent->Draw();

  //c1_2 -> SaveAs("totalOccupancy_Boole_R2.pdf");
  c2_2->SaveAs("occupancyMap_Boole_R2.pdf");
  c1p_2->SaveAs("occupancy_Boole_R2.pdf");
  // c1p_2 -> SaveAs("occupancy_Boole_R2.C");

  f->Close();
}
