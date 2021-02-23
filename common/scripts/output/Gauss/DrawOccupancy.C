#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

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
  TFile *f = TFile::Open(TString(dir1 + "/Gauss-Histo.root"));

  TH1D *R1OverallNH = (TH1D *)f->Get("RICHG4HISTOSET5/363");
  TH1D *R1PmtOccup = (TH1D *)f->Get("RICHG4HISTOSET5/365");
  TH1D *R1PmtOccupPercent = (TH1D *)R1PmtOccup->Clone("R1PmtOccupPerCent");
  TH2D *R1XY = (TH2D *)f->Get("RICHG4HISTOSET5/361");

  TH1D *R2OverallNH = (TH1D *)f->Get("RICHG4HISTOSET5/383");
  TH1D *R2PmtOccup = (TH1D *)f->Get("RICHG4HISTOSET5/385");
  TH1D *R2PmtOccupPercent = (TH1D *)R2PmtOccup->Clone("R2PmtOccupPerCent");
  TH2D *R2XY = (TH2D *)f->Get("RICHG4HISTOSET5/381");

  //normalize
  Int_t numEvR1 = R1OverallNH->GetEntries();
  std::cout << " Number of events Rich1= " << numEvR1 << std::endl;
  R1PmtOccup->Scale(1.0 / (numEvR1 * 1.0));
  R1XY->Scale(1.0 / (numEvR1 * 1.0));
  R1PmtOccupPercent->Scale(1.0 / (numEvR1 * 1.0) * 100.0 / 64.0);

  TH1D *R1PmtOccupNormToMax = (TH1D *)R1PmtOccupPercent->Clone("R1PmtOccupNormToMax");
  Int_t occupancyMaxR1 = R1PmtOccupPercent->GetMaximum();
  R1PmtOccupNormToMax->Scale(1.0 / occupancyMaxR1);

  Int_t numEvR2 = R2OverallNH->GetEntries();
  std::cout << " Number of events Rich2= " << numEvR2 << std::endl;
  R2PmtOccup->Scale(1.0 / (numEvR2 * 1.0));
  R2XY->Scale(1.0 / (numEvR2 * 1.0));
  R2PmtOccupPercent->Scale(1.0 / (numEvR2 * 1.0) * 100.0 / 64.0);

  TH1D *R2PmtOccupNormToMax = (TH1D *)R2PmtOccupPercent->Clone("R2PmtOccupNormToMax");
  Int_t occupancyMaxR2 = R2PmtOccupPercent->GetMaximum();
  R2PmtOccupNormToMax->Scale(1.0 / occupancyMaxR2);

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
  TCanvas *c1n = new TCanvas("c1n", "Rich1 Pmt Occupancy Normalized to 1", canvasSizeX, canvasSizeY);
  R1PmtOccupNormToMax->SetTitle("RICH1 PMT Occupancy Normalized to 1");
  R1PmtOccupNormToMax->Draw();

  //c1 -> SaveAs("365.pdf");
  c2->SaveAs("361.pdf");
  c1p->SaveAs("365_percent.pdf");
  //c1n -> SaveAs("365_norm.pdf");
  //c1p -> SaveAs("365_percent.C");
  //c1n -> SaveAs("365_norm.C");

  TCanvas *c1_2 = new TCanvas("c1_2", "Rich2 Pmt Occupancy", canvasSizeX, canvasSizeY);
  R2PmtOccup->Draw();
  TCanvas *c2_2 = new TCanvas("c2_2", "Rich2 XY Pmt ", canvasSizeX, canvasSizeY);
  c2_2->SetRightMargin(rightMargin);
  R2XY->Draw("colz");
  TCanvas *c1p_2 = new TCanvas("c1p_2", "Rich2 Pmt Occupancy in Percent", canvasSizeX, canvasSizeY);
  R2PmtOccupPercent->SetTitle("RICH2 PMT Occupancy in Percent");
  R2PmtOccupPercent->Draw();
  TCanvas *c1n_2 = new TCanvas("c1n_2", "Rich2 Pmt Occupancy Normalized to 1", canvasSizeX, canvasSizeY);
  R2PmtOccupNormToMax->SetTitle("RICH2 PMT Occupancy Normalized to 1");
  R2PmtOccupNormToMax->Draw();

  //c1_2 -> SaveAs("385.pdf");
  c2_2->SaveAs("381.pdf");
  c1p_2->SaveAs("385_percent.pdf");
  //c1n_2 -> SaveAs("385_norm.pdf");
  //c1p_2 -> SaveAs("385_percent.C");
  //c1n_2 -> SaveAs("385_norm.C");

  // detector occupancy
  {
    TCanvas *c3_1 = new TCanvas("c3_1", "Rich1 : Detector occupancy", canvasSizeX, canvasSizeY);
    // R1OverallNH->Scale(1.0 / (numEvR1 * 1.0));
    R1OverallNH->Draw();
    c3_1->SaveAs("363.pdf");

    TCanvas *c3_2 = new TCanvas("c3_2", "Rich2 : Detector occupancy", canvasSizeX, canvasSizeY);
    // R2OverallNH->Scale(1.0 / (numEvR2 * 1.0));
    R2OverallNH->Draw();
    c3_2->SaveAs("383.pdf");
  }

  f->Close();
}
