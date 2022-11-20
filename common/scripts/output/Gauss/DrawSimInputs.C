#include <iostream>

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include "../../utils/Utils.C"
#include "../../utils/Styles.C"

using namespace RooFit;

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
    const unsigned short rebinN{};
    const unsigned short flagNormalise{};

    // set y-axis range
    const float axisValueMin{};
    const float axisValueMax{};

    PlotData(const std::string &iHistName, const std::string &iPlotName, const std::string &iPlotTitle, const std::string &iAxisXLabel, const std::string &iAxisYLabel, const unsigned short iRebinN = 1, const unsigned short iFlagNormalise = 0, const float &iAxisValueMin = 0., const float &iAxisValueMax = 0.) : histName(iHistName), plotName(iPlotName), plotTitle(iPlotTitle), axisXLabel(iAxisXLabel), axisYLabel(iAxisYLabel), rebinN(iRebinN), flagNormalise(iFlagNormalise), axisValueMin(iAxisValueMin), axisValueMax(iAxisValueMax){};
};

std::vector<PlotData> prepareListOfPlots()
{
    std::vector<PlotData> plots = {
        // refractive index distribution
        PlotData(std::string("RICHG4RefIndHISTOSET6/100"), "refIndex_R1", "Refractive index|Rich1, Gauss", "(n-1) #times 10^{4}", "(1/N) Entries / (bin)", 1, 1),
        PlotData(std::string("RICHG4RefIndHISTOSET6/200"), "refIndex_R2", "Refractive index|Rich2, Gauss", "(n-1) #times 10^{5}", "(1/N) Entries / (bin)", 1, 1),

        // control check for normalisation (check using tests with a single event)
        PlotData(std::string("RICHG4RefIndHISTOSET6/nominalQE"), "nominalQE_normCheck", "Nominal PMT QE|Simulation input", "Wavelength [nm]", "Nominal quantum eff. [%]", 1, 0, 0., 45.),

        // similation inputs
        PlotData(std::string("RICHG4RefIndHISTOSET6/nominalQE"), "nominalQE", "Nominal PMT QE|Simulation input", "Wavelength [nm]", "Nominal quantum eff. [%]", 1, 2, 0., 45.),
        PlotData(std::string("RICHG4RefIndHISTOSET6/reflectivitySpherical"), "reflectivitySpherical", "R1 spherical mirrors reflectivity|Simulation input", "Wavelength [nm]", "Reflectivity", 1, 2, 0.5, 1.0),
        PlotData(std::string("RICHG4RefIndHISTOSET6/reflectivityFlat"), "reflectivityFlat", "R1 flat mirrors reflectivity|Simulation input", "Wavelength [nm]", "Reflectivity", 1, 2, 0.5, 1.0),
        PlotData(std::string("RICHG4RefIndHISTOSET6/refIndex_C4F10"), "refIndex_C4F10", "C4F10 refractive index|Simulation input", "Wavelength [nm]", "(n-1) #times 10^{4}", 1, 2, 13, 14.6),
        PlotData(std::string("RICHG4RefIndHISTOSET6/refIndexCkv_C4F10"), "refIndexCkv_C4F10", "C4F10 refractive index|Simulation input", "Wavelength [nm]", "(n-1) #times 10^{4}", 1, 2, 13, 14.6),
        PlotData(std::string("RICHG4RefIndHISTOSET6/refIndex_CF4"), "refIndex_CF4", "CF4 refractive index|Simulation input", "Wavelength [nm]", "(n-1) #times 10^{4}", 1, 2, 3.6, 5.0),
        PlotData(std::string("RICHG4RefIndHISTOSET6/refIndexCkv_CF4"), "refIndexCkv_CF4", "CF4 refractive index|Simulation input", "Wavelength [nm]", "(n-1) #times 10^{4}", 1, 2, 3.6, 5.0),
        PlotData(std::string("RICHG4RefIndHISTOSET6/refIndex_CO2"), "refIndex_CO2", "CO2 refractive index|Simulation input", "Wavelength [nm]", "(n-1) #times 10^{4}", 1, 2, 3.6, 5.0),
        PlotData(std::string("RICHG4RefIndHISTOSET6/refIndexCkv_CO2"), "refIndexCkv_CO2", "CO2 refractive index|Simulation input", "Wavelength [nm]", "(n-1) #times 10^{4}", 1, 2, 3.6, 5.0)

    };

    return plots;
}

// ------------- end of configuration ------------- //

void DrawSimInputsGeneric(const PlotData &plot, TFile *&inputFile, const unsigned int &flagStyle = 0)
{
    // set style
    Rich::Styles::setStyle(flagStyle);

    // specific settings for the current plot
    gStyle->SetPadLeftMargin(0.17);
    gStyle->SetTitleOffset(1.15, "Y");
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

    // histogram processing
    if (plot.rebinN != 1)
    {
        currentHist->Rebin(plot.rebinN);
    }
    if (plot.flagNormalise == 1)
    {
        currentHist->Scale(1. / currentHist->GetEntries());
    }
    else if (plot.flagNormalise == 2)
    {
        // normalisation for sim inputs (should be only filled at initialisation) (keep an eye on this)
        const float normFactor = (float)currentHist->GetEntries() / currentHist->GetNbinsX();
        currentHist->Scale(1. / normFactor);
    }

    // draw
    if (plot.axisValueMin > 0. || plot.axisValueMax > 0.)
    {
        currentHist->SetAxisRange(plot.axisValueMin, plot.axisValueMax, "Y");
    }

    currentHist->SetMarkerColor(Rich::Styles::getColorPrimary());
    currentHist->SetLineColor(Rich::Styles::getColorPrimary());
    currentHist->DrawCopy("HIST C");

    // add descritpion (optional)
    if (Rich::Styles::flagShowDescription(flagStyle))
    {
        auto plotDescription = Rich::Styles::makePlotDescription(0.038, 0.54, 0.97, 1.04);
        Rich::Utils::addMultilineText(plot.plotTitle, plotDescription, '|');

        plotDescription->Draw();
        currentCanvas.Update();
    }

    // save results
    currentCanvas.SaveAs(".pdf");
}

void DrawSimInputs(const std::string &fileDir, const std::string &fileName, const unsigned int &flagStyle = Rich::Styles::defaultStyleFlag)
{
    // open input file
    auto f = TFile::Open(TString(fileDir + "/" + fileName));

    // draw required plots
    auto plots = prepareListOfPlots();
    for (auto const &plot : plots)
    {
        DrawSimInputsGeneric(plot, f, flagStyle);
    }

    // cleanup
    f->Close();
}
