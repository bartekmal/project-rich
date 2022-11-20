#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>

#include "../../utils/Utils.C"
#include "../../utils/Styles.C"

// ----------------- configuration ---------------- //

// config histograms
// (0 - 3; 0.95 - 2.2)
const float axisValueMin{0.8};
const float axisValueMax{1.5};

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

std::vector<PlotData> plots = {
    // R1
    PlotData("occupancyPd_Boole_R1", "occupancyPd_Boole_R1", "occupancyPd_Boole_R1_ratio", "Detected channel occupancy (av. per PD);Rich1, Boole (ratio)", "PD ID", "Av. ch. occupancy per PD (ratio)", Rich::Styles::axisValueMaxNeutral, false, false, false, false, false),
    PlotData("occupancyChannel_Boole_R1", "occupancyChannel_Boole_R1", "occupancyChannel_Boole_R1_ratio", "Detected channel occupancy (map);Rich1, Boole (ratio)", "x [mm]", "y [mm]", Rich::Styles::axisValueMaxNeutral, false, false, false, true, false),
    // R2
    PlotData("occupancyPd_Boole_R2", "occupancyPd_Boole_R2", "occupancyPd_Boole_R2_ratio", "Detected channel occupancy (av. per PD);Rich2, Boole (ratio)", "PD ID", "Av. ch. occupancy per PD (ratio)", Rich::Styles::axisValueMaxNeutral, false, false, false, false, false),
    PlotData("occupancyChannel_Boole_R2", "occupancyChannel_Boole_R2", "occupancyChannel_Boole_R2_ratio", "Detected channel occupancy (map);Rich2, Boole (ratio)", "x [mm]", "y [mm]", Rich::Styles::axisValueMaxNeutral, false, false, false, true, false),
};

// ------------- end of configuration ------------- //

void DrawOccupancyRatioGeneric(const PlotData &plot, TFile *&inputFile1, TFile *&inputFile2, const unsigned int &flagStyle = 0)
{
  // set style
  Rich::Styles::setStyle(flagStyle);

  // specific settings for the current plot
  if (plot.isOccupancyMap)
  {
    gStyle->SetPadLeftMargin(0.16);
    gStyle->SetTitleOffset(1.12, "Y");
    gStyle->SetPadRightMargin(0.12);
    gStyle->SetPalette(kRainBow);
    gStyle->SetOptStat(0);
  }
  gROOT->ForceStyle();

  // prepare canvas
  const auto canvasSizeX = plot.isOccupancyMap ? Rich::Styles::defaultCanvasSizeX * 1.1f : Rich::Styles::defaultCanvasSizeX;
  const auto canvasSizeY = Rich::Styles::defaultCanvasSizeY;
  auto currentCanvas = TCanvas(plot.plotName.c_str(), plot.plotTitle.c_str(), canvasSizeX, canvasSizeY);

  // get histograms
  auto currentHistMain = (TH1 *)(Rich::Utils::getValidHist(inputFile1, plot.histNameMain));
  auto currentHistRef = (TH1 *)(Rich::Utils::getValidHist(inputFile2, plot.histNameRef));
  if (!currentHistMain || !currentHistRef)
  {
    return;
  }
  auto currentHist = (TH1 *)currentHistMain->Clone(plot.plotName.c_str());
  currentHist->GetXaxis()->SetTitle(plot.axisXLabel.c_str());
  currentHist->GetYaxis()->SetTitle(plot.axisYLabel.c_str());

  // proces the result
  currentHist->Divide(currentHistRef);

  // normalise if needed
  if (plot.isNormalised)
  {
    currentHist->Scale(1.0 / currentHist->GetMaximum());
  }

  // specific settings
  currentHist->SetAxisRange(axisValueMin, axisValueMax, plot.isOccupancyMap ? "Z" : "Y");

  // draw
  if (plot.axisValueMax > 0.)
  {
    currentHist->SetAxisRange(0, plot.axisValueMax, plot.isOccupancyMap ? "Z" : "Y");
  }
  if (plot.axisXLogScale)
  {
    currentCanvas.SetLogx();
  }
  if (plot.axisValueLogScale)
  {
    currentHist->SetAxisRange(Rich::Styles::defaultZeroForLogScale, (plot.axisValueMax > 0.) ? plot.axisValueMax : currentHist->GetMaximum() * Rich::Styles::defaultFactorForMaxValueLogScale, plot.isOccupancyMap ? "Z" : "Y");
    if (plot.isOccupancyMap)
    {
      currentCanvas.SetLogz();
    }
    else
    {
      currentCanvas.SetLogy();
    }
  }

  if (plot.isOccupancyMap)
  {
    currentHist->DrawCopy("colz");
  }
  else
  {
    currentHist->SetMarkerColor(Rich::Styles::getColorSecondary());
    currentHist->SetLineColor(Rich::Styles::getColorPrimary());
    currentHist->SetMarkerSize(0.6);
    currentHist->SetLineWidth(1);
    currentHist->DrawCopy();
  }

  // add descritpion (optional)
  if (Rich::Styles::flagShowDescription(flagStyle))
  {
    auto plotDescription = plot.isOccupancyMap ? Rich::Styles::makePlotDescription(0.049, 0.67, 0.9775, 1.04) : Rich::Styles::makePlotDescription();
    Rich::Utils::addMultilineText(plot.plotTitle, plotDescription);

    plotDescription->Draw();
    currentCanvas.Update();
  }

  // save results
  currentCanvas.SaveAs(".pdf");
  // currentCanvas.SaveAs(".C");
}

void DrawOccupancyRatio(const std::string &filePath1, const std::string &filePath2, const unsigned int &flagStyle = Rich::Styles::defaultStyleFlag)
{
  // open input files
  auto f1 = TFile::Open(filePath1.c_str());
  auto f2 = TFile::Open(filePath2.c_str());

  // draw required plots
  for (auto const &plot : plots)
  {
    DrawOccupancyRatioGeneric(plot, f1, f2, flagStyle);
  }

  // cleanup
  f1->Close();
  f2->Close();
}
