// ---------- docs -----------//
// Helper functions
// ---------- end of docs -----------//

#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <iostream>
#include <string>
namespace Rich
{
      namespace Utils
      {
            // get a fresh copy (clone) of the given object in a ROOT file
            inline TObject *getObjCopy(TFile *&inputFile, const std::string &objName)
            {
                  if (!inputFile)
                  {
                        return nullptr;
                  }
                  const auto objTmp = inputFile->Get(objName.c_str());
                  if (!objTmp)
                  {
                        return nullptr;
                  }
                  else
                  {
                        // make a unique name
                        return objTmp->Clone((inputFile->GetName() + std::string("_") + objName).c_str());
                  }
            }

            // get a valid (existing and non-empty) histogram from a file
            inline TH1 *getValidHist(TFile *&inputFile, const std::string &objName)
            {
                  const auto tmpHist = (TH1 *)(getObjCopy(inputFile, objName));

                  if (!tmpHist)
                  {
                        std::cout << "Histogram not found: " << objName.c_str() << std::endl;
                        return nullptr;
                  }
                  else if (tmpHist->GetEntries() == 0)
                  {
                        std::cout << "Histogram is empty: " << objName.c_str() << std::endl;
                        return nullptr;
                  }
                  else
                  {
                        return tmpHist;
                  }
            }

            // split string and add to TPaveText as lines
            inline void addMultilineText(const std::string &textToAdd, TPaveText *&paveText, const char delimiter = ';')
            {
                  std::istringstream tmpStringStream(textToAdd.c_str());
                  std::string tmpString;
                  while (std::getline(tmpStringStream, tmpString, delimiter))
                  {
                        paveText->AddText(tmpString.c_str());
                  }
            }
      }
}

#endif /* !UTILS_H */