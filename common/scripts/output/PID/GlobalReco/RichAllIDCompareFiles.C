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

void RichAllIDCompareFiles(const std::string dir1, const std::string dir2)
{
  const std::string dirBaseRichScripts( gSystem->Getenv("RICH_BASE_SCRIPTS_GLOBAL_RECO") );
  gROOT->ProcessLine( std::string( ".x "+dirBaseRichScripts+"/RichKaonIDCompareFiles.C(\""+dir1+"\",\""+dir2+"\")" ).c_str() );
  gROOT->ProcessLine( std::string( ".x "+dirBaseRichScripts+"/RichProtonIDCompareFiles.C(\""+dir1+"\",\""+dir2+"\")" ).c_str() );
  gROOT->ProcessLine( std::string( ".x "+dirBaseRichScripts+"/RichPionIDCompareFiles.C(\""+dir1+"\",\""+dir2+"\")" ).c_str() );
}
