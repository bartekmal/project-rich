#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include "../../utils/Utils.C"
#include "../../utils/Styles.C"

// ----------------- configuration ---------------- //

// config histograms
//

// plots to make
struct PlotData
{
    // input / output info
    const std::string histName{};
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

    PlotData(const std::string &iHistName, const std::string &iPlotName, const std::string &iPlotTitle, const std::string &iAxisXLabel, const std::string &iAxisYLabel, const float &iAxisValueMax = Rich::Styles::axisValueMaxNeutral, const bool &iAxisXLogScale = false, const bool &iAxisValueLogScale = false, const bool &iIsNormalised = false) : histName(iHistName), plotName(iPlotName), plotTitle(iPlotTitle), axisXLabel(iAxisXLabel), axisYLabel(iAxisYLabel), axisValueMax(iAxisValueMax), axisXLogScale(iAxisXLogScale), axisValueLogScale(iAxisValueLogScale), isNormalised(iIsNormalised){};
};

const std::string histDir = "RICH/DIGI/PDRESPONSE/";

std::vector<PlotData> plots = {
    PlotData(histDir + "Rich1 : Input signal", "inputSignal_R1", "Rich1: CLARO input signal;(normalised to max value)", "#electrons [Me-]", "Entries (normalised)", 1.2, false, false, true),
    PlotData(histDir + "Rich1 : Time of acquisition", "TOA_R1", "Rich1: Deposit time-of-acquisition;(normalised to max value)", "TOA [ns]", "Entries (normalised)", 10., false, true, true),
    PlotData(histDir + "Rich1 : Time over threshold", "TOT_R1", "Rich1: Deposit time-over-threshold;(normalised to max value)", "TOT [ns]", "Entries (normalised)", 1.2, false, false, true),
    PlotData(histDir + "Rich2 : Input signal", "inputSignal_R2", "Rich2: CLARO input signal;(normalised to max value)", "#electrons [Me-]", "Entries (normalised)", 1.2, false, false, true),
    PlotData(histDir + "Rich2 : Time of acquisition", "TOA_R2", "Rich2: Deposit time-of-acquisition;(normalised to max value)", "TOA [ns]", "Entries (normalised)", 10., false, true, true),
    PlotData(histDir + "Rich2 : Time over threshold", "TOT_R2", "Rich2: Deposit time-over-threshold;(normalised to max value)", "TOT [ns]", "Entries (normalised)", 1.2, false, false, true),
};

// ------------- end of configuration ------------- //

void DrawPdResponseGeneric(const PlotData &plot, TFile *&inputFile, const unsigned int &flagStyle = 0)
{
    // set style
    Rich::Styles::setStyle(flagStyle);

    // specific settings for the current plot
    if (plot.axisValueLogScale)
    {
        gStyle->SetPadLeftMargin(0.16);
        gStyle->SetTitleOffset(1.12, "Y");
        gROOT->ForceStyle();
    }

    // prepare canvas
    const auto canvasSizeX = Rich::Styles::defaultCanvasSizeX;
    const auto canvasSizeY = Rich::Styles::defaultCanvasSizeY;
    auto currentCanvas = TCanvas(plot.plotName.c_str(), plot.plotTitle.c_str(), canvasSizeX, canvasSizeY);

    // get histogram
    auto currentHist = (TH1D *)(Rich::Utils::getValidHist(inputFile, plot.histName));
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
        currentHist->SetAxisRange(Rich::Styles::defaultZeroForLogScaleNormalised, (plot.axisValueMax > 0.) ? plot.axisValueMax : currentHist->GetMaximum() * Rich::Styles::defaultFactorForMaxValueLogScale, "Y");
        currentCanvas.SetLogy();
    }

    currentHist->SetMarkerColor(Rich::Styles::getColorPrimary());
    currentHist->SetLineColor(Rich::Styles::getColorPrimary());
    currentHist->DrawCopy();

    // add descritpion (optional)
    if (Rich::Styles::flagShowDescription(flagStyle))
    {
        auto plotDescription = Rich::Styles::makePlotDescription(0.038, 0.55, 0.925, 1.04);
        Rich::Utils::addMultilineText(plot.plotTitle, plotDescription);

        plotDescription->Draw();
        currentCanvas.Update();
    }

    // save
    currentCanvas.SaveAs(".pdf");
}

void DrawPdResponse(const std::string &fileDir, const std::string &fileName, const unsigned int &flagStyle = Rich::Styles::defaultStyleFlag)
{
    // open input file
    auto f = TFile::Open(TString(fileDir + "/" + fileName));

    // draw required plots
    for (auto const &plot : plots)
    {
        DrawPdResponseGeneric(plot, f, flagStyle);
    }

    // cleanup
    f->Close();
}
