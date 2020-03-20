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

void RichProtonIDCompareFiles(const std::string dir1, const std::string dir2, const std::string title1 = "with SIN", const std::string title2 = "no SIN")
{

  // Default Config Object
  GlobalPID::Configuration defaultConfig;

  const Long64_t nTracks = 1e6;

  const double GeV(1000);

  // Histo range
  defaultConfig.useFixedGraphRange = true;
  //defaultConfig.minGraphX = 80;
  //defaultConfig.maxGraphX = 100;
  //defaultConfig.minGraphY = 40;
  //defaultConfig.maxGraphY = 80;
  // Stepping options
  defaultConfig.maxCut      = 35;
  defaultConfig.nSteps      = 100;
  defaultConfig.minMisIDeff = 1.0;
  // Momentum range
  defaultConfig.minP      = 3   * GeV;
  defaultConfig.maxP      = 100 * GeV;
  defaultConfig.minPt     = 0.5 * GeV;
  defaultConfig.maxPt     = 100 * GeV;
  // track selection
  defaultConfig.trackType = GlobalPID::Long;
  //defaultConfig.trackType = GlobalPID::Upstream;
  //defaultConfig.trackType = GlobalPID::Downstream;
  // detector selection
  defaultConfig.mustHaveAnyRICH = true;
  
  using PlotData = std::vector< std::tuple<std::string,std::string,Color_t> >;

  // colours...
  // kBlack kRed-6 kBlue+1 kGreen+2 kYellow+3 kRed+1 kMagenta+2 kCyan+2

  const PlotData plotdata = 
  {
    std::make_tuple ( dir1+"/Brunel-Ntuple.root", title1, kRed-6 ),
    std::make_tuple ( dir2+"/Brunel-Ntuple.root", title2, kBlue+1 ),
  };

  //protonID VS pionMisID
  {
    // make a pid object
    auto pid = std::make_unique<GlobalPID>();
    pid->config             = defaultConfig;
    // Plot Type
    pid->config.title     = "RICH Proton ID VS Pion MisID (richDLL)";
    pid->config.idType    = GlobalPID::Proton;
    pid->config.misidType = GlobalPID::Pion;
    pid->config.var1      = GlobalPID::richDLLp;
    pid->config.var2      = GlobalPID::richDLLpi;
    // Plot ranges
    pid->config.minGraphX = 76;
    pid->config.maxGraphX = 100;
    pid->config.minGraphY = 1;
    pid->config.maxGraphY = 20;

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
	pid->config.subtitle    = title;
	pid->config.superImpose = ( iPlot != 0 );
	pid->config.color       = color;
	// create the plot
	pid->makeCurve(nTracks);
	++iPlot;
      }
  
    // save the figures
    pid->saveFigures();
    //pid->saveFigures("root");
  }

  //protonID VS kaonMisID
  {
    // make a pid object
    auto pid = std::make_unique<GlobalPID>();
    pid->config             = defaultConfig;
    // Plot Type
    pid->config.title     = "RICH Proton ID VS Kaon MisID (richDLL)";
    pid->config.idType    = GlobalPID::Proton;
    pid->config.misidType = GlobalPID::Kaon;
    pid->config.var1      = GlobalPID::richDLLp;
    pid->config.var2      = GlobalPID::richDLLk;
    // Plot ranges
    pid->config.minGraphX = 40;
    pid->config.maxGraphX = 100;
    pid->config.minGraphY = 2;
    pid->config.maxGraphY = 70;

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
	pid->config.subtitle    = title;
	pid->config.superImpose = ( iPlot != 0 );
	pid->config.color       = color;
	// create the plot
	pid->makeCurve(nTracks);
	++iPlot;
      }
  
    // save the figures
    pid->saveFigures();
    //pid->saveFigures("root");
  }
}
