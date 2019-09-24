/*****************************************************************************\
* (c) Copyright 2000-2018 CERN for the benefit of the LHCb Collaboration      *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "COPYING".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

#include <tuple>
#include <vector>
#include <memory>

#include "GlobalPID.C"

void RichKaonIDCompareFiles(const std::string dir1, const std::string dir2, const int richNr = 0, const std::string title1 = "PMTArray geometry", const std::string title2 = "reference")
{

  // make a pid object
  auto pid = std::make_unique<GlobalPID>();

  // Default Config Object
  GlobalPID::Configuration defaultConfig;

  const Long64_t nTracks = 1e6;

  const double GeV(1000);

  // Histo range
  defaultConfig.useFixedGraphRange = true;
  defaultConfig.minGraphX = 80;
  defaultConfig.maxGraphX = 100;
  defaultConfig.minGraphY = 1;
  defaultConfig.maxGraphY = 20;
  //defaultConfig.minGraphX = 80;
  //defaultConfig.maxGraphX = 100;
  //defaultConfig.minGraphY = 40;
  //defaultConfig.maxGraphY = 80;
  // Stepping options
  defaultConfig.maxCut      = 35;
  defaultConfig.nSteps      = 100;
  defaultConfig.minMisIDeff = 1.0;
  // Momentum range
  if ( richNr == 0 ){
    defaultConfig.minP      = 3   * GeV;
    defaultConfig.maxP      = 100 * GeV;
  } else if ( richNr == 1 ){
    defaultConfig.minP      = 3   * GeV;
    defaultConfig.maxP      = 20 * GeV;
  } else if ( richNr == 2 ){
    defaultConfig.minP      = 35  * GeV;
    defaultConfig.maxP      = 100 * GeV;
  } else if ( richNr == 3 ){
    defaultConfig.minP      = 20  * GeV;
    defaultConfig.maxP      = 35 * GeV;
  } else {
    std::cout << "Unknown nrRich configuration (choose 0,1,2)" << std::endl;
    return;
  }
  defaultConfig.minPt     = 0.5 * GeV;
  defaultConfig.maxPt     = 100 * GeV;
  // track selection
  defaultConfig.trackType = GlobalPID::Long;
  //defaultConfig.trackType = GlobalPID::Upstream;
  //defaultConfig.trackType = GlobalPID::Downstream;
  // detector selection
  defaultConfig.mustHaveAnyRICH = true;
  // Plot Type
  defaultConfig.title     = "RICH Kaon ID";
  defaultConfig.idType    = GlobalPID::Kaon;
  defaultConfig.misidType = GlobalPID::Pion;
  defaultConfig.var1      = GlobalPID::richDLLk;
  defaultConfig.var2      = GlobalPID::richDLLpi;

  using PlotData = std::vector< std::tuple<std::string,std::string,Color_t> >;

  // colours...
  // kBlack kRed-6 kBlue+1 kGreen+2 kYellow+3 kRed+1 kMagenta+2 kCyan+2

  const PlotData plotdata = 
  {
    std::make_tuple ( dir1+"/Brunel-Ntuple.root", title1, kRed ),
    std::make_tuple ( dir2+"/Brunel-Ntuple.root", title2, kBlue ),
  };

  unsigned int iPlot = 0;
  for ( const auto& pd : plotdata )
  {
    // ROOT file
    const auto & fname = std::get<0>(pd);
    // title
    const auto & title = std::get<1>(pd);
    // colour
    const auto & color = std::get<2>(pd);
    pid->loadTTree( fname );
    pid->config             = defaultConfig;
    pid->config.subtitle    = title;
    pid->config.superImpose = ( iPlot != 0 );
    pid->config.color       = color;
    // create the plot
    pid->makeCurve(nTracks);
    ++iPlot;
  }
  
  // save the figures
  pid->saveFigures();
  
}
