#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooBifurGauss.h"
#include "RooPlot.h"

using namespace RooFit;

// ----------------- configuration ---------------- //

// plots to make
struct PlotData
{
    const std::string histName{};
    const std::string plotTitle{};
    const std::string plotName{};

    const unsigned short rebinN{};
    const unsigned short fitFlag{};

    const float fitRangeMin{};
    const float fitRangeMax{};

    PlotData(const std::string &iHistName, const std::string &iPlotTitle, const std::string &iPlotName, const unsigned short iRebinN = 1, const unsigned short iFitFlag = 0, const float &iFitRangeMin = 0., const float &iFitRangeMax = 0.) : histName(iHistName), plotTitle(iPlotTitle), plotName(iPlotName), rebinN(iRebinN), fitFlag(iFitFlag), fitRangeMin(iFitRangeMin), fitRangeMax(iFitRangeMax){};
};

std::vector<PlotData> plots = {
    // R1
    PlotData("RICHG4HISTOSET2/160", "Photon yield - R1", "yield_R1", 1, 1, 40., 75.), // range before sim input updates: 25, 60
    PlotData("RICHG4HISTOSET4/1500A", "Single photon resolution [rad] (chromatic) - R1", "1500", 1, 2, 0.0505, 0.0545),
    PlotData("RICHG4HISTOSET4/1530", "Single photon resolution [rad] (emission) - R1", "1530"),
    PlotData("RICHG4HISTOSET4/1540", "Single photon resolution [rad] (pixel) - R1", "1540", 2),
    PlotData("RICHG4HISTOSET4/1501", "Single photon resolution [rad] (overall) - R1", "resOverall_R1", 2, 1, 0.049, 0.055),

    // R2 (R+H)
    PlotData("RICHG4HISTOSET2/180", "Photon yield - R2", "yield_R2", 1, 1, 20., 45.), // range before sim input updates: 12, 35
    PlotData("RICHG4HISTOSET4/1700A", "Single photon resolution [rad] (chromatic) - R2 (global)", "1700", 1, 2, 0.0285, 0.0315),
    PlotData("RICHG4HISTOSET4/1730", "Single photon resolution [rad] (emission) - R2 (global)", "1730"),
    PlotData("RICHG4HISTOSET4/1740", "Single photon resolution [rad] (pixel) - R2 (global)", "1740", 2),
    PlotData("RICHG4HISTOSET4/1701", "Single photon resolution [rad] (overall) - R2 (global)", "resOverall_R2", 2, 1, 0.0286, 0.0308),

    // R2 (R only)
    PlotData("RICHG4HISTOSET2/180", "Photon yield - R2", "yield_R2", 1, 1, 20., 45.), // range before sim input updates: 12, 35
    PlotData("RICHG4HISTOSET4/1700A_std", "Single photon resolution [rad] (chromatic) - R2 (PMT-R)", "1700_RType", 1, 2, 0.0285, 0.0315),
    PlotData("RICHG4HISTOSET4/1730_std", "Single photon resolution [rad] (emission) - R2 (PMT-R)", "1730_RType"),
    PlotData("RICHG4HISTOSET4/1740_std", "Single photon resolution [rad] (pixel) - R2 (PMT-R)", "1740_RType", 2),
    PlotData("RICHG4HISTOSET4/1701_std", "Single photon resolution [rad] (overall) - R2 (PMT-R)", "resOverall_R2_RType", 2, 1, 0.0286, 0.0308),

    // R2 (H only)
    PlotData("RICHG4HISTOSET2/180", "Photon yield - R2", "yield_R2", 1, 1, 20., 45.), // range before sim input updates: 12, 35
    PlotData("RICHG4HISTOSET4/1700A_grand", "Single photon resolution [rad] (chromatic) - R2 (PMT-H)", "1700_HType", 1, 2, 0.0285, 0.0315),
    PlotData("RICHG4HISTOSET4/1730_grand", "Single photon resolution [rad] (emission) - R2 (PMT-H)", "1730_HType"),
    PlotData("RICHG4HISTOSET4/1740_grand", "Single photon resolution [rad] (pixel) - R2 (PMT-H)", "1740_HType", 2),
    PlotData("RICHG4HISTOSET4/1701_grand", "Single photon resolution [rad] (overall) - R2 (PMT-H)", "resOverall_R2_HType", 2, 1, 0.0286, 0.0308),
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
    // prepare canvas
    auto currentCanvas = TCanvas(plot.plotName.c_str(), plot.plotTitle.c_str(), canvasSizeX, canvasSizeY);

    // get histogram
    auto currentHist = (TH1D *)inputFile->Get(plot.histName.c_str());
    currentHist->SetTitle(plot.plotTitle.c_str());

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
        if (plot.histName == "RICHG4HISTOSET2/160" || plot.histName == "RICHG4HISTOSET2/180")
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
        model.paramOn(resFrame, Layout(0.20, 0.4, 0.6));

        gPad->SetLeftMargin(0.15);
        resFrame->GetYaxis()->SetTitleOffset(1.6);
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
            const float effSigma = 1 / nSum * (nLeft * sigL.getVal() + nRight * sigR.getVal());
            // const float effSigma2 = 1 / nSum * sqrt(pow(nLeft * sigL.getVal(), 2) + pow(nRight * sigR.getVal(), 2));

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
        currentHist->Draw();
    }

    // transform resolutions to mrad
    if (!(plot.histName == "RICHG4HISTOSET2/160" || plot.histName == "RICHG4HISTOSET2/180"))
    {
        resultToSave *= 1000;
    }

    // save results
    currentCanvas.SaveAs(".pdf");

    std::ofstream outputFile("results.dat", ios::app);
    outputFile << std::fixed << std::setprecision(4) << resultToSave << "\t";
    outputFile.close();
}

void DrawPerformance(const std::string &fileDir, const std::string &fileName = std::string("Gauss-Histo.root"))
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
