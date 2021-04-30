#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include "/afs/cern.ch/work/b/bmalecki/RICH_Upgrade/common/scripts/utils/Utils.C"

#include "/afs/cern.ch/work/b/bmalecki/RICH_Upgrade/common/scripts/utils/Utils.C"
#include "/afs/cern.ch/work/b/bmalecki/RICH_Upgrade/common/scripts/utils/Styles.C"

// ----------------- configuration ---------------- //

// plots to make
struct PlotData
{
    // input / output info
    const std::string histName{};
    const std::string plotName{};
    const std::string plotTitle{};
    const std::string axisXLabel{};
    const std::string axisYLabel{};

    // info for the histograms processing
    const float fitRangeMin{};
    const float fitRangeMax{};

    PlotData(const std::string &iHistName, const std::string &iPlotName, const std::string &iPlotTitle, const std::string &iAxisXLabel, const std::string &iAxisYLabel, const float &iFitRangeMin, const float &iFitRangeMax) : histName(iHistName), plotName(iPlotName), plotTitle(iPlotTitle), axisXLabel(iAxisXLabel), axisYLabel(iAxisYLabel), fitRangeMin(iFitRangeMin), fitRangeMax(iFitRangeMax){};
};

std::vector<PlotData> plots = {
    PlotData("RICH/RiCKMCResLong/Rich1Gas/ckResTrue", "res_reconstructed_R1", "Single-photon resolution;Rich1, Brunel (MC true)", "#Delta(Cherenkov theta) [rad]", "Entries / (bin)", -0.0015, 0.0015),
    PlotData("RICH/RiCKMCResLong/Rich2Gas/ckResTrue", "res_reconstructed_R2", "Single-photon resolution;Rich2, Brunel (MC true)", "#Delta(Cherenkov theta) [rad]", "Entries / (bin)", -0.001, 0.001),
};

// ------------- end of configuration ------------- //

void DrawPerformanceGeneric(const PlotData &plot, TFile *&inputFile, const unsigned int &flagStyle = 0)
{
    // set style
    Rich::Styles::setStyle(flagStyle);

    // specific settings for the current plot
        gStyle->SetPadTopMargin(0.06);
    gStyle->SetPadLeftMargin(0.15);
    gStyle->SetTitleOffset(1.05, "Y");
    gStyle->SetStatW(0.18);
    gROOT->ForceStyle();

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

    //fit
    currentHist->Fit("gaus", "", "", plot.fitRangeMin, plot.fitRangeMax);

    // draw
    currentHist->SetMarkerColor(Rich::Styles::getColorPrimary());
    currentHist->SetLineColor(Rich::Styles::getColorPrimary());
    currentHist->DrawCopy();

    // add descritpion (optional)
    if (Rich::Styles::flagShowDescription(flagStyle))
    {
        auto plotDescription = Rich::Styles::makePlotDescription(0.1125, 0.5, 0.97, 1.04);
        Rich::Utils::addMultilineText(plot.plotTitle, plotDescription);

        plotDescription->Draw();
        currentCanvas.Update();
    }

    // save
    currentCanvas.SaveAs(".pdf");
}

void DrawPerformance(const std::string &fileDir, const std::string &fileName, const unsigned int &flagStyle = Rich::Styles::defaultStyleFlag)
{
    // open input file
    auto f = TFile::Open(TString(fileDir + "/" + fileName));

    // draw required plots
    for (auto const &plot : plots)
    {
        DrawPerformanceGeneric(plot, f, flagStyle);
    }

    // cleanup
    f->Close();
}
