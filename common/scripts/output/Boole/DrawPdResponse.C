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

    const bool flagLogScale{};

    PlotData(const std::string &iHistName, const std::string &iPlotTitle, const std::string &iPlotName, const bool &iFlagLogScale = false) : histName(iHistName), plotTitle(iPlotTitle), plotName(iPlotName), flagLogScale(iFlagLogScale){};
};

const std::string pdResponsePath = "RICH/DIGI/PDRESPONSE/";

std::vector<PlotData> plots = {
    PlotData(pdResponsePath + "Rich1 : Input signal", "Rich1 : CLARO input signal [Me-]", "inputSignal_R1"),
    PlotData(pdResponsePath + "Rich1 : Time of acquisition", "Rich1 : Deposit time of acquisition [ns]", "TOA_R1"),
    PlotData(pdResponsePath + "Rich1 : Time over threshold", "Rich1 : Deposit time over threshold [ns]", "TOT_R1"),
    PlotData(pdResponsePath + "Rich2 : Input signal", "Rich2 : CLARO input signal [Me-]", "inputSignal_R2"),
    PlotData(pdResponsePath + "Rich2 : Time of acquisition", "Rich2 : Deposit time of acquisition [ns]", "TOA_R2"),
    PlotData(pdResponsePath + "Rich2 : Time over threshold", "Rich2 : Deposit time over threshold [ns]", "TOT_R2"),
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

void DrawPdResponseGeneric(const PlotData &plot, TFile *&inputFile)
{
    // get histogram
    auto currentHist = (TH1D *)inputFile->Get(plot.histName.c_str());
    if (!currentHist) return;

    // draw
    auto currentCanvas = TCanvas(plot.plotName.c_str(), plot.plotTitle.c_str(), canvasSizeX, canvasSizeY);
    currentHist->SetTitle(plot.plotTitle.c_str());
    // log scale
    if (plot.flagLogScale) {
      currentCanvas.SetLogy();
    }
    currentHist->Draw();

    // save
    currentCanvas.SaveAs(".pdf");
}

void DrawPdResponse(const std::string &fileDir, const std::string &fileName)
{
    // style
    setStyle();

    // open input file
    auto f = TFile::Open(TString(fileDir + "/" + fileName));

    // draw required plots
    for (auto const &plot : plots)
    {
        DrawPdResponseGeneric(plot, f);
    }

    // cleanup
    f->Close();
}
