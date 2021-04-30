#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooBifurGauss.h"
#include "RooPlot.h"

#include "/afs/cern.ch/work/b/bmalecki/RICH_Upgrade/common/scripts/utils/Utils.C"
#include "/afs/cern.ch/work/b/bmalecki/RICH_Upgrade/common/scripts/utils/Styles.C"

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
    const unsigned short fitFlag{};

    const float fitRangeMin{};
    const float fitRangeMax{};

    PlotData(const std::string &iHistName, const std::string &iPlotName, const std::string &iPlotTitle, const std::string &iAxisXLabel, const std::string &iAxisYLabel, const unsigned short iRebinN = 1, const unsigned short iFitFlag = 0, const float &iFitRangeMin = 0., const float &iFitRangeMax = 0.) : histName(iHistName), plotName(iPlotName), plotTitle(iPlotTitle), axisXLabel(iAxisXLabel), axisYLabel(iAxisYLabel), rebinN(iRebinN), fitFlag(iFitFlag), fitRangeMin(iFitRangeMin), fitRangeMax(iFitRangeMax){};
};

std::vector<PlotData> prepareListOfPlots(const bool &isParticleGun)
{

    const unsigned int rebinFactorForSignal = 2;

    std::vector<PlotData> plots = {
        // R1
        PlotData(std::string("RICHG4HISTOSET2/") + (isParticleGun ? "160" : "158"), "yield_R1", "Av. photon yield|Rich1, Gauss", "#photon hits per saturated track", "Entries / (bin)", 1, 1, 20, 100), // range before sim input updates: 25, 60
        PlotData(std::string("RICHG4HISTOSET4/1500A") + (isParticleGun ? "" : "_highQuality"), "1500", "Single-photon resolution (chromatic)|Rich1, Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 1 : rebinFactorForSignal), 2, 0.0505, 0.0545),
        PlotData(std::string("RICHG4HISTOSET4/1530") + (isParticleGun ? "" : "_highQuality"), "1530", "Single-photon resolution (emission point)|Rich1, Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 1 : rebinFactorForSignal)),
        PlotData(std::string("RICHG4HISTOSET4/1540") + (isParticleGun ? "" : "_highQuality"), "1540", "Single-photon resolution (pixel)|Rich1, Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal)),
        PlotData(std::string("RICHG4HISTOSET4/1501") + (isParticleGun ? "" : "_highQuality"), "resOverall_R1", "Single-photon resolution (overall)|Rich1, Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 1, 0.0480, 0.0570),
        PlotData(std::string("RICHG4HISTOSET4/1501") + (isParticleGun ? "" : "_highQuality"), "resOverall_bifurcated_R1", "Single-photon resolution (overall)|Rich1, Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 2, 0.0480, 0.0570),
        PlotData(std::string("RICHG4HISTOSET4/1560"), "1560", "Single-photon resolution (overall - gen)|Rich1, Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 1, -0.004, 0.004),

        // R2 (R+H)
        PlotData(std::string("RICHG4HISTOSET2/") + (isParticleGun ? "180" : "178"), "yield_R2", "Av. photon yield|Rich2, Gauss", "#photon hits per saturated track", "Entries / (bin)", 1, 1, 0, 60), // range before sim input updates: 12, 35
        PlotData(std::string("RICHG4HISTOSET4/1700A") + (isParticleGun ? "" : "_highQuality"), "1700", "Single-photon resolution (chromatic)|Rich2 (global), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 1 : rebinFactorForSignal), 2, 0.0285, 0.0315),
        PlotData(std::string("RICHG4HISTOSET4/1730") + (isParticleGun ? "" : "_highQuality"), "1730", "Single-photon resolution (emission point)|Rich2 (global), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 1 : rebinFactorForSignal)),
        PlotData(std::string("RICHG4HISTOSET4/1740") + (isParticleGun ? "" : "_highQuality"), "1740", "Single-photon resolution (pixel)|Rich2 (global), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal)),
        PlotData(std::string("RICHG4HISTOSET4/1701") + (isParticleGun ? "" : "_highQuality"), "resOverall_R2", "Single-photon resolution (overall)|Rich2 (global), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 1, 0.0260, 0.0340),
        PlotData(std::string("RICHG4HISTOSET4/1701") + (isParticleGun ? "" : "_highQuality"), "resOverall_bifurcated_R2", "Single-photon resolution (overall)|Rich2 (global), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 2, 0.0260, 0.0340),
        PlotData(std::string("RICHG4HISTOSET4/1760"), "1760", "Single-photon resolution (overall - gen)|Rich2, Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 1, -0.004, 0.004),

        // R2 (R only)
        PlotData(std::string("RICHG4HISTOSET2/") + (isParticleGun ? "180" : "178"), "yield_R2", "Av. photon yield|Rich2, Gauss", "#photon hits per saturated track", "Entries / (bin)", 1, 1, 0, 60), // range before sim input updates: 12, 35
        PlotData(std::string("RICHG4HISTOSET4/1700A_std") + (isParticleGun ? "" : "_highQuality"), "1700_RType", "Single-photon resolution (chromatic)|Rich2 (PMT-R), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 1 : rebinFactorForSignal), 2, 0.0285, 0.0315),
        PlotData(std::string("RICHG4HISTOSET4/1730_std") + (isParticleGun ? "" : "_highQuality"), "1730_RType", "Single-photon resolution (emission point)|Rich2 (PMT-R), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 1 : rebinFactorForSignal)),
        PlotData(std::string("RICHG4HISTOSET4/1740_std") + (isParticleGun ? "" : "_highQuality"), "1740_RType", "Single-photon resolution (pixel)|Rich2 (PMT-R), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal)),
        PlotData(std::string("RICHG4HISTOSET4/1701_std") + (isParticleGun ? "" : "_highQuality"), "resOverall_R2_RType", "Single-photon resolution (overall)|Rich2 (PMT-R), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 1, 0.0260, 0.0340),
        PlotData(std::string("RICHG4HISTOSET4/1701_std") + (isParticleGun ? "" : "_highQuality"), "resOverall_bifurcated_R2_RType", "Single-photon resolution (overall)|Rich2 (PMT-R), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 2, 0.0260, 0.0340),
        PlotData(std::string("RICHG4HISTOSET4/1760_std"), "1760_std", "Single-photon resolution (overall - gen)|Rich2 (PMT-R), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 1, -0.004, 0.004),

        // R2 (H only)
        PlotData(std::string("RICHG4HISTOSET2/") + (isParticleGun ? "180" : "178"), "yield_R2", "Av. photon yield|Rich2, Gauss", "#photon hits per saturated track", "Entries / (bin)", 1, 1, 0, 60), // range before sim input updates: 12, 35
        PlotData(std::string("RICHG4HISTOSET4/1700A_grand") + (isParticleGun ? "" : "_highQuality"), "1700_HType", "Single-photon resolution (chromatic)|Rich2 (PMT-H), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 1 : rebinFactorForSignal), 2, 0.0285, 0.0315),
        PlotData(std::string("RICHG4HISTOSET4/1730_grand") + (isParticleGun ? "" : "_highQuality"), "1730_HType", "Single-photon resolution (emission point)|Rich2 (PMT-H), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 1 : rebinFactorForSignal)),
        PlotData(std::string("RICHG4HISTOSET4/1740_grand") + (isParticleGun ? "" : "_highQuality"), "1740_HType", "Single-photon resolution (pixel)|Rich2 (PMT-H), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal)),
        PlotData(std::string("RICHG4HISTOSET4/1701_grand") + (isParticleGun ? "" : "_highQuality"), "resOverall_R2_HType", "Single-photon resolution (overall)|Rich2 (PMT-H), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 1, 0.0260, 0.0340),
        PlotData(std::string("RICHG4HISTOSET4/1701_grand") + (isParticleGun ? "" : "_highQuality"), "resOverall_bifurcated_R2_HType", "Single-photon resolution (overall)|Rich2 (PMT-H), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 2, 0.0260, 0.0340),
        PlotData(std::string("RICHG4HISTOSET4/1760_grand"), "1760_grand", "Single-photon resolution (overall - gen)|Rich2 (PMT-H), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", (isParticleGun ? 2 : 2 * rebinFactorForSignal), 1, -0.004, 0.004),
    };

    // typical yields for signal (additional info)
    if (!isParticleGun)
    {
        plots.emplace_back(std::string("RICHG4HISTOSET2/160"), "yield_typical_R1", "Av. photon yield (typical for signal)|Rich1, Gauss", "#photon hits per saturated track", "Entries / (bin)", 1, 1, 40., 75.); // range before sim input updates: 25, 60
        plots.emplace_back(std::string("RICHG4HISTOSET2/180"), "yield_typical_R2", "Av. photon yield (typical for signal)|Rich2, Gauss", "#photon hits per saturated track", "Entries / (bin)", 1, 1, 20., 45.); // range before sim input updates: 12, 35
    }

    // alternatives for the overall resolution for signal (additional info)
    if (!isParticleGun)
    {
        plots.emplace_back(std::string("RICHG4HISTOSET4/1501"), "resOverall_all_R1", "Single-photon resolution (overall)|Rich1, Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", 2 * rebinFactorForSignal, 1, 0.0480, 0.0570);
        plots.emplace_back(std::string("RICHG4HISTOSET4/1501_highMomentum"), "resOverall_highMomentum_R1", "Single-photon resolution (overall)|Rich1, Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", 2 * rebinFactorForSignal, 1, 0.0480, 0.0570);

        plots.emplace_back(std::string("RICHG4HISTOSET4/1701"), "resOverall_all_R2", "Single-photon resolution (overall)|Rich2 (global), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", 2 * rebinFactorForSignal, 1, 0.0260, 0.0340);
        plots.emplace_back(std::string("RICHG4HISTOSET4/1701_highMomentum"), "resOverall_highMomentum_R2", "Single-photon resolution (overall)|Rich2 (global), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", 2 * rebinFactorForSignal, 1, 0.0260, 0.0340);

        plots.emplace_back(std::string("RICHG4HISTOSET4/1701_std"), "resOverall_all_R2_RType", "Single-photon resolution (overall)|Rich2 (PMT-R), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", 2 * rebinFactorForSignal, 1, 0.0260, 0.0340);
        plots.emplace_back(std::string("RICHG4HISTOSET4/1701_std_highMomentum"), "resOverall_highMomentum_R2_RType", "Single-photon resolution (overall)|Rich2 (PMT-R), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", 2 * rebinFactorForSignal, 1, 0.0260, 0.0340);

        plots.emplace_back(std::string("RICHG4HISTOSET4/1701_grand"), "resOverall_all_R2_HType", "Single-photon resolution (overall)|Rich2 (PMT-H), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", 2 * rebinFactorForSignal, 1, 0.0260, 0.0340);
        plots.emplace_back(std::string("RICHG4HISTOSET4/1701_grand_highMomentum"), "resOverall_highMomentum_R2_HType", "Single-photon resolution (overall)|Rich2 (PMT-H), Gauss", "Cherenkov theta angle [rad]", "Entries / (bin)", 2 * rebinFactorForSignal, 1, 0.0260, 0.0340);
    }

    return plots;
}

// ------------- end of configuration ------------- //

void DrawPerformanceGeneric(const PlotData &plot, TFile *&inputFile, const unsigned int &flagStyle = 0)
{
    // set style
    Rich::Styles::setStyle(flagStyle);

    // specific settings for the current plot
    gStyle->SetPadLeftMargin(0.18);
    gStyle->SetTitleOffset(1.30, "Y");

    if (plot.fitFlag != 0)
    {
        gStyle->SetStatW(0.13);
    }

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

    // rebin the histogram if required
    if (plot.rebinN != 1)
    {
        currentHist->Rebin(plot.rebinN);
        // currentHist->Scale(1.0 / plot.rebinN);
    }

    // get a result (perform fit if needed)
    float resultToSave{0.};
    if (plot.fitFlag == 0)
    {
        resultToSave = currentHist->GetStdDev();
    }
    // fit with gaussian
    else if (plot.fitFlag == 1)
    {
        const auto fitResult = currentHist->Fit("gaus", "S", "", plot.fitRangeMin, plot.fitRangeMax);

        // special case for yields
        if (plot.histName.rfind("RICHG4HISTOSET2/", 0) == 0)
        {
            // gauss mean
            resultToSave = fitResult->Parameter(1);
        }
        else
        {
            // gauss sigma
            resultToSave = fitResult->Parameter(2);
        }
    }
    // fit with bifurcated gaussian (using RooFit)
    else if (plot.fitFlag == 2)
    {
        // create model
        RooRealVar res("res", "", plot.fitRangeMin, plot.fitRangeMax);

        RooRealVar mean("mean", "#mu", (plot.fitRangeMin + plot.fitRangeMax) / 2., plot.fitRangeMin, plot.fitRangeMax);
        RooRealVar sigL("sigL", "#sigma_{L}", 0.00015, 0.0, 1.0);
        RooRealVar sigR("sigR", "#sigma_{R}", 0.00070, 0.0, 1.0);

        RooBifurGauss model("model", "model", res, mean, sigL, sigR);

        // get data
        RooDataHist dh("dh", "dh", res, Import(*currentHist));

        // fit
        model.fitTo(dh);

        // prepare plot
        RooPlot *resFrame = res.frame(Title(plot.plotTitle.c_str()));
        dh.plotOn(resFrame);
        model.plotOn(resFrame);

        // print stats
        model.paramOn(resFrame, Layout(0.21, 0.5, 0.45));

        resFrame->Draw();

        // calculate effective sigma by taking into account both slopes with weights based on the #events on each peak side
        {
            const unsigned short distanceInSigma = 3;
            const float rangeLow = mean.getVal() - distanceInSigma * sigL.getVal();
            const float rangeHigh = mean.getVal() + distanceInSigma * sigR.getVal();

            const unsigned int iMeanBin = floor((mean.getVal() - currentHist->GetBinLowEdge(1)) / currentHist->GetBinWidth(1)) + 1;
            const unsigned int iRangeLowBin = floor((rangeLow - currentHist->GetBinLowEdge(1)) / currentHist->GetBinWidth(1)) + 1;
            const unsigned int iRangeHighBin = floor((rangeHigh - currentHist->GetBinLowEdge(1)) / currentHist->GetBinWidth(1)) + 1;

            // offset by a number of bins from the peak to treat both slopes in the same way
            const unsigned short offsetFromPeakInBins = 1;
            const float nFullPeak = currentHist->Integral(iRangeLowBin, iRangeHighBin);
            const float nLeft = currentHist->Integral(iRangeLowBin, iMeanBin - offsetFromPeakInBins);
            const float nRight = currentHist->Integral(iMeanBin + offsetFromPeakInBins, iRangeHighBin);
            const float nSum = nLeft + nRight;

            // choose a way to calculated the effective sigma (~weighted mean; linear sum is a worse-case scenario than the quadratic one)
            // const float effSigma = 1 / nSum * (nLeft * sigL.getVal() + nRight * sigR.getVal());
            // const float effSigma = 1 / nSum * sqrt(pow(nLeft * sigL.getVal(), 2) + pow(nRight * sigR.getVal(), 2));
            const float effSigma = 0.5 * (sigL.getVal() + sigR.getVal());
            // const float effSigma = 0.5 * sqrt(pow( sigL.getVal(), 2) + pow(sigR.getVal(), 2));

            // debug
            // {
            //     std::cout << "Hello there!" << mean.getVal() << "\t" << rangeLow << "\t" << rangeHigh << "\t" << std::endl;
            //     std::cout << "Hello there!" << iMeanBin << "\t" << iRangeLowBin << "\t" << iRangeHighBin << "\t" << std::endl;
            //     std::cout << "Hello there!" << nFullPeak << "\t" << nLeft << "\t" << nRight << "\t" << nSum << "\t" << std::endl;
            //     std::cout << "Hello there!" << effSigma << "\t" << effSigma2 << "\t" << std::endl;

            //     std::cout << "Hello there!" << currentHist->Integral(iRangeLowBin, iRangeLowBin) << "\t" << std::endl; // both limits inclusive
            // }

            resultToSave = effSigma;
        }
    }

    // draw (if not already done in RooFit)
    if (plot.fitFlag != 2)
    {
        currentHist->SetMarkerColor(Rich::Styles::getColorPrimary());
        currentHist->SetLineColor(Rich::Styles::getColorPrimary());
        currentHist->DrawCopy();
    }

    // add descritpion (optional)
    if (Rich::Styles::flagShowDescription(flagStyle))
    {
        auto plotDescription = Rich::Styles::makePlotDescription(0.038, 0.54, 0.97, 1.04);
        Rich::Utils::addMultilineText(plot.plotTitle, plotDescription, '|');

        plotDescription->Draw();
        currentCanvas.Update();
    }

    // transform resolutions to mrad
    if (!(plot.histName.rfind("RICHG4HISTOSET2/", 0) == 0))
    {
        resultToSave *= 1000;
    }

    // save results
    currentCanvas.SaveAs(".pdf");

    std::ofstream outputFile("results.dat", ios::app);
    outputFile << std::fixed << std::setprecision(4) << resultToSave << "\t";
    outputFile.close();
}

void DrawPerformance(const std::string &fileDir, const std::string &fileName = std::string("Gauss-Histo.root"), const bool &isParticleGun = false, const unsigned int &flagStyle = Rich::Styles::defaultStyleFlag)
{
    // open input file
    auto f = TFile::Open(TString(fileDir + "/" + fileName));

    // draw required plots
    auto plots = prepareListOfPlots(isParticleGun);
    for (auto const &plot : plots)
    {
        DrawPerformanceGeneric(plot, f, flagStyle);
    }

    // cleanup
    f->Close();
}
