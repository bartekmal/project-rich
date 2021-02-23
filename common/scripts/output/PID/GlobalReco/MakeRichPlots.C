/*****************************************************************************\
* (c) Copyright 2018 CERN for the benefit of the LHCb Collaboration           *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

#include <iostream>
#include <sstream>
#include <string>

#include "TPad.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TFile.h"
#include "TTree.h"
#include "TCut.h"

void MakeRichPlots(const std::string filePath1)
{

  const std::string imageType = "pdf";
  gStyle->SetOptStat(0000);

  const std::vector<std::string> nametags =
      {
          //"Scale0.25",
          //"Scale0.5",
          //"Scale0.75",
          //"Scale0.85",
          //"Scale1.0"
          // "Scale-R1_0.75-R2_0.40",
          // "Scale-R1_0.75-R2_0.45",
          // "Scale-R1_0.75-R2_0.50",
          // "Scale-R1_0.75-R2_0.75",
          // "Scale-R1_0.75-R2_0.83",
          // "Scale-R1_0.75-R2_0.90",
          // "Scale-R1_0.75-R2_1.00",
          "Scale-R1_1.00-R2_1.00"};

  for (const auto &nametag : nametags)
  {

    // load the file and TTree
    TFile *f = TFile::Open(filePath1.c_str());
    if (!f)
      continue;
    TTree *tree = (TTree *)gDirectory->Get("ChargedProtoTuple/protoPtuple");
    if (!tree)
      continue;

    TCut detOK = "RichUsedAero || RichUsedR1Gas || RichUsedR2Gas";

    std::string trackSelS = "TrackType==3 && TrackP>3000 && TrackP<100000 && TrackChi2PerDof<5 && TrackPt >500.0";
    //trackSelS += " && TrackGhostProb<0.9"; // add ghost prob cut
    //trackSelS += " && TrackLikelihood>-40"; // Likelihood
    //trackSelS += " && (TrackCloneDist>5000 || TrackCloneDist<0)"; // clone distance
    TCut trackSel = trackSelS.c_str();

    TCut realPr = "abs(MCParticleType) == 2212";
    TCut realK = "abs(MCParticleType) == 321";
    TCut realPi = "abs(MCParticleType) == 211";

    TCut prAboveThres = "RichAbovePrThres";
    TCut kAboveThres = "RichAboveKaThres";
    TCut piAboveThres = "RichAbovePiThres";
    TCut TrackRCut = "fabs(1000.0*tan(asin(TrackPt/TrackP))) < 300.0";

    TCanvas *c = new TCanvas(nametag.c_str(), nametag.c_str(), 1400, 1000);

    //    const std::vector<double> cuts = { -10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10 };
    const std::vector<double> cuts = {0, 2, 4};

    // Open a PDF file
    // const std::string pdfFile = nametag + "-RichPID-Geomv7Sipm." + imageType;
    const std::string pdfFile = nametag + "-RichPID-StdUpgrade." + imageType;
    c->SaveAs((pdfFile + "[").c_str());

    // Kaon ID plots (kaon vs pion)
    for (const auto &cut : cuts)
    {
      std::ostringstream cC;
      cC << cut;

      tree->Draw(("(RichDLLk>" + cC.str() + "?100:0):TrackP>>kIDEff").c_str(),
                 detOK && realK && trackSel && kAboveThres && TrackRCut, "prof");
      tree->Draw(("(RichDLLk>" + cC.str() + "?100:0):TrackP>>piMisIDEff").c_str(),
                 detOK && realPi && trackSel && piAboveThres && TrackRCut, "prof");
      TH1F *kIDEff = (TH1F *)gDirectory->Get("kIDEff");
      TH1F *piMisIDEff = (TH1F *)gDirectory->Get("piMisIDEff");

      if (kIDEff && piMisIDEff)
      {
        kIDEff->SetTitle(("DLLk>" + cC.str() + " && " + trackSelS).c_str());
        kIDEff->GetXaxis()->SetTitle("Momentum / MeV/c");
        kIDEff->GetYaxis()->SetTitle("Efficiency / %");
        kIDEff->Draw();
        kIDEff->SetMarkerColor(kRed);
        kIDEff->SetLineColor(kRed);

        piMisIDEff->Draw("SAME");
        piMisIDEff->SetMarkerColor(kBlue);
        piMisIDEff->SetLineColor(kBlue);

        c->SaveAs(pdfFile.c_str());
      }
      else
      {
        std::cerr << "Error making kaon plots" << std::endl;
      }
    }

    // Proton ID plots (proton vs kaon)
    for ( const auto& cut : cuts )
    {
      std::ostringstream cC;
      cC << cut;

      tree->Draw( ("(RichDLLp>"+cC.str()+"?100:0):TrackP>>prIDEff").c_str(),
                  detOK && realPr && trackSel && prAboveThres, "prof" );
      tree->Draw( ("(RichDLLp>"+cC.str()+"?100:0):TrackP>>kMisIDEff").c_str(),
                  detOK && realK && trackSel && kAboveThres, "prof" );
      TH1F * prIDEff    = (TH1F*) gDirectory->Get("prIDEff");
      TH1F * kMisIDEff = (TH1F*) gDirectory->Get("kMisIDEff");

      if ( prIDEff && kMisIDEff )
      {
        prIDEff->SetTitle( ( "DLLp>"+cC.str() + " && " + trackSelS ).c_str() );
        prIDEff->GetXaxis()->SetTitle( "Momentum / MeV/c" );
        prIDEff->GetYaxis()->SetTitle( "Efficiency / %" );
        prIDEff->Draw();
        prIDEff->SetMarkerColor(kRed);
        prIDEff->SetLineColor(kRed);

        kMisIDEff->Draw("SAME");
        kMisIDEff->SetMarkerColor(kBlue);
        kMisIDEff->SetLineColor(kBlue);

        c->SaveAs( pdfFile.c_str() );
      }
      else
      {
        std::cerr << "Error making proton plots" << std::endl;
      }

    }

    // // Proton ID plots (proton vs pion)
    // for ( const auto& cut : cuts )
    // {
    //   std::ostringstream cC;
    //   cC << cut;

    //   tree->Draw( ("(RichDLLp>"+cC.str()+"?100:0):TrackP>>prIDEff").c_str(),
    //               detOK && realPr && trackSel && prAboveThres, "prof" );
    //   tree->Draw( ("(RichDLLp>"+cC.str()+"?100:0):TrackP>>piMisIDEff").c_str(),
    //               detOK && realPi && trackSel && piAboveThres, "prof" );
    //   TH1F * prIDEff    = (TH1F*) gDirectory->Get("prIDEff");
    //   TH1F * piMisIDEff = (TH1F*) gDirectory->Get("piMisIDEff");

    //   if ( prIDEff && piMisIDEff )
    //   {
    //     prIDEff->SetTitle( ( "DLLp>"+cC.str() + " && " + trackSelS ).c_str() );
    //     prIDEff->GetXaxis()->SetTitle( "Momentum / MeV/c" );
    //     prIDEff->GetYaxis()->SetTitle( "Efficiency / %" );
    //     prIDEff->Draw();
    //     prIDEff->SetMarkerColor(kRed);
    //     prIDEff->SetLineColor(kRed);

    //     piMisIDEff->Draw("SAME");
    //     piMisIDEff->SetMarkerColor(kBlue);
    //     piMisIDEff->SetLineColor(kBlue);

    //     c->SaveAs( pdfFile.c_str() );
    //   }
    //   else
    //   {
    //     std::cerr << "Error making proton plots" << std::endl;
    //   }

    // }

    // close the PDF file
    c->SaveAs((pdfFile + "]").c_str());

    // clean up
    f->Close();
    delete c;
  }
}
