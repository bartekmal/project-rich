#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include "../../utils/Utils.C"
#include "../../utils/Styles.C"

// ----------------- configuration ---------------- //

const std::string histFile{"output.root"};

// config histograms
const float axisValueMaxR1 = 50.0;
const float axisValueMaxR2 = 20.0;
const float axisValueMaxForMapR1 = 50.0;
const float axisValueMaxForMapR2 = 20.0;

struct PlotData
{
  // input / output info
  const std::string histNameMain{};
  const std::string histNameRef{};
  const std::string plotName{};
  const std::string plotTitle{};
  const std::string axisXLabel{};
  const std::string axisYLabel{};

  // plot config
  const float axisValueMax{};
  const bool axisXLogScale{};
  const bool axisValueLogScale{};

  // info for the histograms processing
  const bool isNormalised{};
  const bool isOccupancyMap{};
  const bool isDetectorOccupancy{};

  PlotData(const std::string &iHistNameMain, const std::string &iHistNameRef, const std::string &iPlotName, const std::string &iPlotTitle, const std::string &iAxisXLabel, const std::string &iAxisYLabel, const float &iAxisValueMax = Rich::Styles::axisValueMaxNeutral, const bool &iAxisXLogScale = false, const bool &iAxisValueLogScale = false, const bool &iIsNormalised = false, const bool &iIsOccupancyMap = false, const bool &iIsDetectorOccupancy = false) : histNameMain(iHistNameMain), histNameRef(iHistNameRef), plotName(iPlotName), plotTitle(iPlotTitle), axisXLabel(iAxisXLabel), axisYLabel(iAxisYLabel), axisValueMax(iAxisValueMax), axisXLogScale(iAxisXLogScale), axisValueLogScale(iAxisValueLogScale), isNormalised(iIsNormalised), isOccupancyMap(iIsOccupancyMap), isDetectorOccupancy(iIsDetectorOccupancy){};
};

const std::string histDir = "RICHG4HISTOSET5/";

std::vector<PlotData> plots = {
    // R1
    PlotData(histDir + "365", histDir + "363", "occupancyPd_Gauss_R1", "Av. #channel hits in an event (av. per PD);Rich1, Gauss", "PD ID", "Av. #channel hits per PD [%]", axisValueMaxR1, false, false, false, false, false),
    PlotData(histDir + "365_shortWLRegion", histDir + "363", "occupancyPd_Gauss_R1_shortWLRegion", "Av. #channel hits in an event (av. per PD);Rich1, Gauss (short-WL region)", "PD ID", "Av. #channel hits per PD [%]", axisValueMaxR1, false, false, false, false, false),
    PlotData(histDir + "365_longWLRegion", histDir + "363", "occupancyPd_Gauss_R1_longWLRegion", "Av. #channel hits in an event (av. per PD);Rich1, Gauss (long-WL region)", "PD ID", "Av. #channel hits per PD [%]", axisValueMaxR1, false, false, false, false, false),
    // PlotData(histDir + "365", histDir + "363", "365_norm", "occupancyPd_Gauss_R1", "Av. #channel hits in an event (av. per PD);Rich1, Gauss", "PD ID", "Av. #channel hits in an event per PD (normalised)", 1.1, false, false, true, false, false),
    PlotData(histDir + "361", histDir + "363", "occupancyChannel_Gauss_R1", "Av. #channel hits in an event [%] (map);Rich1, Gauss", "x [mm]", "y [mm]", axisValueMaxForMapR1, false, false, false, true, false),
    PlotData(histDir + "363", histDir + "363", "occupancyDetector_Gauss_R1", "Detector #hits for different events;Rich1, Gauss", "Detector #hits per event", "Entries / (bin)", Rich::Styles::axisValueMaxNeutral, false, false, false, false, true),
    // R2
    PlotData(histDir + "385", histDir + "383", "occupancyPd_Gauss_R2", "Av. #channel hits in an event (av. per PD);Rich2, Gauss", "PD ID", "Av. #channel hits per PD [%]", axisValueMaxR2, false, false, false, false, false),
    PlotData(histDir + "385_shortWLRegion", histDir + "383", "occupancyPd_Gauss_R2_shortWLRegion", "Av. #channel hits in an event (av. per PD);Rich2, Gauss (short-WL region)", "PD ID", "Av. #channel hits per PD [%]", axisValueMaxR2, false, false, false, false, false),
    PlotData(histDir + "385_longWLRegion", histDir + "383", "occupancyPd_Gauss_R2_longWLRegion", "Av. #channel hits in an event (av. per PD);Rich2, Gauss (long-WL region)", "PD ID", "Av. #channel hits per PD [%]", axisValueMaxR2, false, false, false, false, false),
    // PlotData(histDir + "385", histDir + "383", "385_norm","occupancyPd_Gauss_R2", "Av. #channel hits in an event (av. per PD);Rich2, Gauss", "PD ID", "Av. #channel hits in an event per PD (normalised)", 1.1, false, false, true, false, false),
    PlotData(histDir + "381", histDir + "383", "occupancyChannel_Gauss_R2", "Av. #channel hits in an event [%] (map);Rich2, Gauss", "x [mm]", "y [mm]", axisValueMaxForMapR2, false, false, false, true, false),
    PlotData(histDir + "383", histDir + "383", "occupancyDetector_Gauss_R2", "Detector #hits for different events;Rich2, Gauss", "Detector #hits per event", "Entries / (bin)", Rich::Styles::axisValueMaxNeutral, false, false, false, false, true),
};

// ------------- end of configuration ------------- //

void DrawOccupancyGeneric(const PlotData &plot, TFile *&inputFile, const unsigned int &flagStyle = 0)
{
  // set style
  Rich::Styles::setStyle(flagStyle);

  // specific settings for the current plot
  // gROOT->ForceStyle();

  // prepare canvas
  const auto canvasSizeX = Rich::Styles::defaultCanvasSizeX;
  const auto canvasSizeY = Rich::Styles::defaultCanvasSizeY;
  auto currentCanvas = TCanvas(plot.plotName.c_str(), plot.plotTitle.c_str(), canvasSizeX, canvasSizeY);

  // get histograms
  auto currentHistMain = (TH1D *)(Rich::Utils::getValidHist(inputFile, plot.histNameMain));
  auto currentHistRef = (TH1D *)(Rich::Utils::getValidHist(inputFile, plot.histNameRef));
  if (!currentHistMain || !currentHistRef)
  {
    return;
  }
  auto currentHist = (TH1D *)currentHistMain->Clone(plot.plotName.c_str());
  currentHist->GetXaxis()->SetTitle(plot.axisXLabel.c_str());
  currentHist->GetYaxis()->SetTitle(plot.axisYLabel.c_str());

  // proces the result
  const Double_t nEvents = currentHistRef->GetEntries();
  std::cout << "Number of events : " << nEvents << std::endl;
  currentHist->Scale(1.0 / nEvents * 100.0 / 64.0);

  // normalise if needed
  if (plot.isNormalised)
  {
    currentHist->Scale(1.0 / currentHist->GetMaximum());
  }

  // specific settings
  if (plot.histNameMain.find("365") != std::string::npos)
  {
    currentHist->SetAxisRange(0, 2112, "X");
  }
  else if (plot.histNameMain.find("385") != std::string::npos)
  {
    currentHist->SetAxisRange(0, 2304, "X");
  }

  // draw
  if (plot.axisValueMax > 0.)
  {
    currentHist->SetAxisRange(0, plot.axisValueMax, "Y");
  }
  if (plot.axisXLogScale)
  {
    currentCanvas.SetLogx();
  }
  if (plot.axisValueLogScale)
  {
    currentHist->SetAxisRange(Rich::Styles::defaultZeroForLogScale, (plot.axisValueMax > 0.) ? plot.axisValueMax : currentHist->GetMaximum() * Rich::Styles::defaultFactorForMaxValueLogScale, "Y");
    currentCanvas.SetLogy();
  }

  currentHist->SetMarkerColor(Rich::Styles::getColorPrimary());
  currentHist->SetLineColor(Rich::Styles::getColorPrimary());
  currentHist->DrawCopy();

  // add descritpion (optional)
  if (Rich::Styles::flagShowDescription(flagStyle))
  {
    auto plotDescription = Rich::Styles::makePlotDescription();
    Rich::Utils::addMultilineText(plot.plotTitle, plotDescription);

    plotDescription->Draw();
    currentCanvas.Update();
  }

  // save results
  currentCanvas.SaveAs(".pdf");
  // currentCanvas.SaveAs(".C");
  currentHist->Write();
}

void DrawOccupancyMapGeneric(const PlotData &plot, TFile *&inputFile, const unsigned int &flagStyle = 0)
{
  // set style
  Rich::Styles::setStyle(flagStyle);

  // specific settings for the current plot
  gStyle->SetPadLeftMargin(0.16);
  gStyle->SetTitleOffset(1.12, "Y");
  gStyle->SetPadRightMargin(0.105);
  gStyle->SetPalette(kRainBow);
  gStyle->SetOptStat(0);
  gROOT->ForceStyle();

  // prepare canvas
  const auto canvasSizeX = Rich::Styles::defaultCanvasSizeX * 1.1f;
  const auto canvasSizeY = Rich::Styles::defaultCanvasSizeY;
  auto currentCanvas = TCanvas(plot.plotName.c_str(), plot.plotTitle.c_str(), canvasSizeX, canvasSizeY);

  // get histograms
  auto currentHistMain = (TH2D *)(Rich::Utils::getValidHist(inputFile, plot.histNameMain));
  auto currentHistRef = (TH2D *)(Rich::Utils::getValidHist(inputFile, plot.histNameRef));
  if (!currentHistMain || !currentHistRef)
  {
    return;
  }
  auto currentHist = (TH2D *)currentHistMain->Clone(plot.plotName.c_str());
  currentHist->GetXaxis()->SetTitle(plot.axisXLabel.c_str());
  currentHist->GetYaxis()->SetTitle(plot.axisYLabel.c_str());

  // proces the result
  const Double_t nEvents = currentHistRef->GetEntries();
  std::cout << "Number of events : " << nEvents << std::endl;
  currentHist->Scale(1.0 / nEvents * 100.0);

  // specific settings
  const float histRange = 825.0f;
  currentHist->SetAxisRange(-histRange, histRange, "X");
  currentHist->SetAxisRange(-histRange, histRange, "Y");

  // draw
  if (plot.axisValueMax > 0.)
  {
    currentHist->SetAxisRange(0, plot.axisValueMax, "Z");
  }
  if (plot.axisValueLogScale)
  {
    currentHist->SetAxisRange(Rich::Styles::defaultZeroForLogScale, (plot.axisValueMax > 0.) ? plot.axisValueMax : currentHist->GetMaximum() * Rich::Styles::defaultFactorForMaxValueLogScale, "Z");
    currentCanvas.SetLogz();
  }
  currentHist->DrawCopy("colz");

  // add descritpion (optional)
  if (Rich::Styles::flagShowDescription(flagStyle))
  {
    auto plotDescription = Rich::Styles::makePlotDescription(0.049, 0.69, 0.9775, 1.04);
    Rich::Utils::addMultilineText(plot.plotTitle, plotDescription);

    plotDescription->Draw();
    currentCanvas.Update();
  }

  // save results
  currentCanvas.SaveAs(".pdf");
  // currentCanvas.SaveAs(".C");
  currentHist->Write();
}

void DrawDetectorOccupancyGeneric(const PlotData &plot, TFile *&inputFile, const unsigned int &flagStyle = 0)
{
  // set style
  Rich::Styles::setStyle(flagStyle);

  // specific settings for the current plot
  gStyle->SetPadRightMargin(0.070);
  gROOT->ForceStyle();

  // prepare canvas
  const auto canvasSizeX = Rich::Styles::defaultCanvasSizeX;
  const auto canvasSizeY = Rich::Styles::defaultCanvasSizeY;
  auto currentCanvas = TCanvas(plot.plotName.c_str(), plot.plotTitle.c_str(), canvasSizeX, canvasSizeY);

  // get histograms
  auto currentHist = (TH1D *)(Rich::Utils::getValidHist(inputFile, plot.histNameMain));
  if (!currentHist)
  {
    return;
  }
  currentHist->GetXaxis()->SetTitle(plot.axisXLabel.c_str());
  currentHist->GetYaxis()->SetTitle(plot.axisYLabel.c_str());

  // normalise if needed
  if (plot.isNormalised)
  {
    currentHist->Scale(1.0 / currentHist->GetMaximum());
  }

  // draw
  if (plot.axisValueMax > 0.)
  {
    currentHist->SetAxisRange(0, plot.axisValueMax, "Y");
  }
  if (plot.axisXLogScale)
  {
    currentCanvas.SetLogx();
  }
  if (plot.axisValueLogScale)
  {
    currentHist->SetAxisRange(Rich::Styles::defaultZeroForLogScale, (plot.axisValueMax > 0.) ? plot.axisValueMax : currentHist->GetMaximum() * Rich::Styles::defaultFactorForMaxValueLogScale, "Y");
    currentCanvas.SetLogy();
  }

  currentHist->SetMarkerColor(Rich::Styles::getColorPrimary());
  currentHist->SetLineColor(Rich::Styles::getColorPrimary());
  currentHist->DrawCopy();

  // add descritpion (optional)
  if (Rich::Styles::flagShowDescription(flagStyle))
  {
    auto plotDescription = Rich::Styles::makePlotDescription();
    Rich::Utils::addMultilineText(plot.plotTitle, plotDescription);

    plotDescription->Draw();
    currentCanvas.Update();
  }

  // save results
  currentCanvas.SaveAs(".pdf");
  // currentCanvas.SaveAs(".C");
  currentHist->Write();
}

void DrawOccupancy(const std::string &fileDir, const std::string &fileName = std::string("Gauss-Histo.root"), const unsigned int &flagStyle = Rich::Styles::defaultStyleFlag)
{
  // open IO files
  auto f = TFile::Open(TString(fileDir + "/" + fileName));
  auto fOut = TFile::Open(histFile.c_str(), "UPDATE");

  // draw required plots
  for (auto const &plot : plots)
  {
    if (plot.isOccupancyMap)
    {
      DrawOccupancyMapGeneric(plot, f, flagStyle);
    }
    else if (plot.isDetectorOccupancy)
    {
      DrawDetectorOccupancyGeneric(plot, f, flagStyle);
    }
    else
    {
      DrawOccupancyGeneric(plot, f, flagStyle);
    }
  }

  // cleanup
  f->Close();
  fOut->Close();
}
