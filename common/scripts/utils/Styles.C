// ---------- docs -----------//

// This file contains some common styles for plotting histograms (e.g. working, pub, lhcb). The style should be adapted accordingly in the relevant scripts/plots.

// Also some useful templates are given for elements that are common in plots.

// ---------- end of docs -----------//

#ifndef STYLES_H
#define STYLES_H

// ---------- templates -----------//

// style modifications
//gStyle->SetErrorX(0); //  don't suppress the error bar along X
//gStyle->SetPadRightMargin(0.05); // increase for colz plots
//gStyle->SetOptStat("emr");  // show only nent -e , mean - m , rms -r ( 0010 )
//gStyle->SetOptFit(1011); // order is probability, Chi2, errors, parameters
// gStyle->SetStatFontSize(0.05);
// gStyle->SetStatX(0.9);
// gStyle->SetStatY(0.9);
// gStyle->SetStatW(0.25);
// gStyle->SetStatH(0.15);

// colors
// gStyle->SetPalette(kRainBow); // kRainBow / kColorPrintableOnGrey / 104 / kNeon / kVisibleSpectrum
// h->SetMarkerColor(Rich::Styles::getColor(0));
// h->SetLineColor(Rich::Styles::getColor(0)); // for errors
// h->SetHistLineColor(Rich::Styles::getColor(0));
// h->SetHistFillColor(Rich::Styles::getColor(0));
// h->SetFuncColor(Rich::Styles::getColorSecondary()); // for functions

// titles
// h->GetXaxis()->SetTitle("#font[12]{m}(#font[12]{D}^{+}#pi^{#font[122]{-}}) [GeV/#font[12]{c}^{2}]");
// h->GetYaxis()->SetTitle("Entries / (0.5 GeV/#font[12]{c}^{2})");

// axes
// h->GetXaxis()->GetXmin();
// h->GetXaxis()->GetXmax();
// h->GetMinimum();
// h->GetMaximum();
// h->SetMinimum(100);
// h->SetMaximum(350.);
// h->GetXaxis()->SetLimits(0, 10);
// h->SetAxisRange(0, plot.occupancyYMax, "Y");
// c.SetLogx();
// c.SetLogy();

// draw
//   h->DrawCopy("");
//   h->DrawCopy("SAME");

// add LHCb label
// (TPaveText : add text as lines + auto scale the font size, also takes latex; see: https://root.cern.ch/doc/master/classTPaveText.html)
// TPaveText *lhcbName = new TPaveText(gStyle->GetPadLeftMargin() + 0.05,
//                                     0.87 - gStyle->GetPadTopMargin(),
//                                     gStyle->GetPadLeftMargin() + 0.20,
//                                     0.95 - gStyle->GetPadTopMargin(),
//                                     "BRNDC");
// lhcbName->AddText("LHCb"); // "LHCb simulation" / "LHCb preliminary"
// Rich::Utils::splitTitleIntoLines("split;lines;by;colon", lhcbName);
// lhcbName->SetFillColor(0);
// lhcbName->SetTextAlign(12);
// lhcbName->SetBorderSize(0);
// lhcbName->Draw();

// labels
//   TText *lhcbLabel = new TText(0.1,0.1,"text");
//   lhcbLabel->SetTextFont(gStyle->GetTextFont());
//   lhcbLabel->SetTextColor(1);
//   lhcbLabel->SetTextSize(gStyle->GetTextSize());
//   lhcbLabel->SetTextAlign(12);
//   lhcbLabel->Draw();

// (allows for multiple use)
//   TLatex *lhcbLatex = new TLatex();
//   lhcbLatex->SetTextFont(gStyle->GetTextFont());
//   lhcbLatex->SetTextColor(1);
//   lhcbLatex->SetTextSize(gStyle->GetTextSize());
//   lhcbLatex->SetTextAlign(12);
//   lhcbLatex->DrawLatexNDC(0.1,0.1,"text");

// Best is to make titles ourselves:
//   TLatex* tyax = new TLatex(0.06, 0.52, "Entries / (0.5 GeV/#font[12]{c}^{2})");
//   tyax->SetNDC(kTRUE);
//   tyax->SetTextSize(0.05);
//   tyax->SetTextAngle(90.);
//   tyax->Draw();

// legend
//   auto legend = new TLegend(0.65,0.65,0.92,0.92);
//   legend->SetFillColor(0);
//   legend->SetTextSize(0.055);
//   legend->AddEntry(h1fa,"Data","P");
//   legend->AddEntry(h1fd,"Background 2","F");
//   legend->Draw();

// update canvas and styles
// c1->Update();
// c->UseCurrentStyle(); //can be called at any point
// gROOT->ForceStyle(); // needs to be called before the object is created/read from a file

// ---------- end of templates -----------//

namespace Rich
{
      namespace Styles
      {
            // colours ... https://root.cern.ch/doc/master/classTColor.html (from Chris: Rec/Rec/GlobalReco/root/RichKaonIDCompareFiles.C)
            const std::array<Color_t, 11> colors{kBlack, kRed - 6, kBlue + 1, kGreen + 2, kRed + 1, kBlue - 1,
                                                 kMagenta + 2, kYellow + 3, kCyan + 2, kGreen - 5, kGray + 2};

            // from the custom palette
            const unsigned int defaultColorId{2};
            const unsigned int secondaryColorId{4};

            // from the ROOT predefined colors
            const unsigned int defaultBkgColorId{19};

            const unsigned int defaultCanvasSizeX{800};
            const unsigned int defaultCanvasSizeY{800};

            const unsigned int defaultStyleFlag{3};

            const float defaultZeroForLogScale{1.0};
            const float defaultZeroForLogScaleNormalised{1e-4};
            const float defaultFactorForMaxValue{1.1};
            const float defaultFactorForMaxValueLogScale{2.0};

            const float axisValueMaxNeutral = -1.0;

            // get a valid color from the defined palette
            inline Color_t getColor(unsigned int i)
            {
                  // sanitise
                  const auto nrOfColors = colors.size();
                  if (i > nrOfColors - 1)
                  {
                        i %= nrOfColors;
                  }
                  return colors[i];
            }

            inline Color_t getColorPrimary()
            {
                  return getColor(defaultColorId);
            }

            inline Color_t getColorSecondary()
            {
                  return getColor(secondaryColorId);
            }

            // list of styles
            inline void setStyleDefault(); // default style
            inline void setStyleLhcb();    // style for LHCb publications
            inline void setStyleSlides();  // style for public slides (with description etc)
            inline void setStyleWorking(); // normal 'working' style (with stats etc)

            // set style by flag
            inline void setStyle(const unsigned int &flagStyle)
            {
                  if (flagStyle == 3)
                  {
                        setStyleWorking();
                  }
                  else if (flagStyle == 2)
                  {
                        setStyleSlides();
                  }
                  else if (flagStyle == 1)
                  {
                        setStyleLhcb();
                  }
                  else
                  {
                        setStyleDefault();
                  }
            }

            // tell if a plot description should be shown for the given style
            inline unsigned int flagShowDescription(const unsigned int &flagStyle)
            {
                  if (flagStyle >= 2)
                  {
                        return 1;
                  }
                  else
                  {
                        return 0;
                  }
            }

            // tell if the stats / fit info should be shown
            inline unsigned int flagShowStats(const unsigned int &flagStyle)
            {
                  if (flagStyle == 3)
                  {
                        return 1;
                  }
                  else
                  {
                        return 0;
                  }
            }

            // make plot description
            inline TPaveText *makePlotDescription(const float posX1 = 0.038, const float posX2 = 0.58, const float posY1 = 0.925, const float posY2 = 1.04)
            {
                  auto plotDescription = new TPaveText(gStyle->GetPadLeftMargin() + posX1,
                                                       posY1 - gStyle->GetPadTopMargin(),
                                                       gStyle->GetPadLeftMargin() + posX2,
                                                       posY2 - gStyle->GetPadTopMargin(),
                                                       "BRNDC");
                  plotDescription->SetFillColor(defaultBkgColorId);
                  plotDescription->SetTextAlign(22);
                  plotDescription->SetBorderSize(0);

                  return plotDescription;
            }

      }
}

inline void Rich::Styles::setStyleDefault()
{
      gStyle->Reset();
      gStyle->SetOptStat(0);
      gStyle->SetPalette(104);
      gROOT->ForceStyle();
}

inline void Rich::Styles::setStyleLhcb()
{
      gStyle->Reset();
      gROOT->ProcessLine(std::string(".x " + std::string(std::getenv("RICH_BASE_SOFTWARE")) + "/utils/Alexandria/RootTools/LHCbStyle/src/lhcbStyle.C").c_str());
}

inline void Rich::Styles::setStyleSlides()
{
      setStyleLhcb();

      gStyle->SetFuncColor(Rich::Styles::getColorSecondary());
      gStyle->SetFuncWidth(3);

      gROOT->ForceStyle();
}

inline void Rich::Styles::setStyleWorking()
{
      setStyleLhcb();

      gStyle->SetOptStat(1111); // show only nent -e , mean - m , rms -r ( 1110 )
      gStyle->SetOptFit(111);   // order is probability, Chi2, errors, parameters
      gStyle->SetStatBorderSize(1);
      gStyle->SetStatFontSize(0.025);
      gStyle->SetStatX(0.99);
      gStyle->SetStatY(0.99);

      gStyle->SetFuncColor(Rich::Styles::getColorSecondary());
      gStyle->SetFuncWidth(3);

      gROOT->ForceStyle();
}

#endif /* !STYLES_H */