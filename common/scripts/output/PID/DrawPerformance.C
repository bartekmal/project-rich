#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

// ----------------- configuration ---------------- //

// plots to make
struct PlotData
{
    const std::string histName{};
    const std::string plotTitle{};
    const std::string plotName{};

    const float fitRangeMin = 0.;
    const float fitRangeMax = 0.;

    PlotData(const std::string &iHistName, const std::string &iPlotTitle, const std::string &iPlotName, const float &iFitRangeMin, const float &iFitRangeMax) : histName(iHistName), plotTitle(iPlotTitle), plotName(iPlotName), fitRangeMin(iFitRangeMin), fitRangeMax(iFitRangeMax){};
};

std::vector<PlotData> plots = {
    PlotData("RICH/RiCKMCResLong/Rich1Gas/ckResTrue", "Single photon resolution [rad] (Brunel, MC true) - R1", "res_Brunel_R1", -0.0015, 0.0015),
    PlotData("RICH/RiCKMCResLong/Rich2Gas/ckResTrue", "Single photon resolution [rad] (Brunel, MC true) - R2", "res_Brunel_R2", -0.001, 0.001),
};

// style
const auto canvasSizeX = 1200;
const auto canvasSizeY = 1540;
const float rightMargin = 0.125;

void setStyle()
{
    gStyle->SetOptFit(111);
    gStyle->SetOptStat(1111);
    gStyle->SetPalette(104);
}

// ------------- end of configuration ------------- //

void DrawPerformanceGeneric(const PlotData &plot, TFile *&inputFile)
{
    // get histogram
    auto currentHist = (TH1D *)inputFile->Get(plot.histName.c_str());

    //fit
    currentHist->Fit("gaus", "", "", plot.fitRangeMin, plot.fitRangeMax);

    // draw
    auto currentCanvas = TCanvas(plot.plotName.c_str(), plot.plotTitle.c_str(), canvasSizeX, canvasSizeY);
    currentHist->SetTitle(plot.plotTitle.c_str());
    currentHist->Draw();

    // save
    currentCanvas.SaveAs(".pdf");
}

void DrawPerformance(const std::string &fileDir, const std::string &fileName = std::string("Brunel-Histo.root"))
{
    // style
    setStyle();

    // open input file
    auto f = TFile::Open(TString(fileDir + "/" + fileName));

    // draw required plots
    for (auto const &plot : plots)
    {
        DrawPerformanceGeneric(plot, f);
    }

    // cleanup
    f->Close();
}
