#! /usr/bin/env python3

# Built-in/Generic Imports
import os

from importlib.machinery import SourceFileLoader
smartIdHelper = SourceFileLoader('smartId', os.path.join(os.environ['RICH_BASE_SCRIPTS_UTILS'], 'smartId.py')).load_module()

# config
smartIdsInputPath = "input/smartIds/output/RichSmartIDs.txt"

outputPath = "output"
outputPathRich1 = outputPath + "/Rich1/ReadoutConf"
outputPathRich2 = outputPath + "/Rich2/ReadoutConf"

listsToCreateRichSmartId = [
    ["PMTSmartIDs","int","RichSmartID identifiers for each PMT. A bit pack word containing the RICH detector, panel number,PMT Module and PMT Num in Module. For packing details, see the class RichSmartID"]
]

listsToCreateCopyNumber = [
    ["PMTHardwareIDs","int","Hardware identifiers for each PMT. A unique number associated to each PMT (copynumber for now)."],
    ["PMTLevel0IDs","int","L0 ID","L0 board identifiers for each PMT. A unique number associated to each PMT (copynumber for now)."],
    ["PMTCopyNumbers","int","Copy number for each PMT. It should match the Geant4 copy number."]
]
listsToCreateModuleNumber = [
    ["PMTLevel1IDs","int","L1 board identifiers for each PMT (module number for now)."],
    ["PMTLevel1HardwareIDs","int","L1 board hardware (serial number) identifiers for each PMT (module number for now)."],
]
listsToCreatePmtInModuleNumber = [
    ["PMTLevel1InputNums","int","L1 board input numbers for each PMT."]
]
mapsToCreate = [
    ["Level1LogicalToHardwareIDMap-PMT","string","Mapping between logical (first) and hardware (second) ID numbers for each L1 board-PMT "]
]

listsToCreateEmpty = [
    ["InactivePMTs","int","PMT hardware IDs for inactive PMTs"],
    ["InactivePMTListInSmartIDs","int","smartIDs for inactive PMTs. Replaces hwID list "]
]

# global variables
nMaxPmtCopyNumberRich1 = smartIdHelper.pmtCopyNumberMaxR1
nMaxPmtCopyNumberRich2 = smartIdHelper.pmtCopyNumberMaxR2

# helpers

def readRichSmartIds(filePath):
    fileRichSmartIDs = open(filePath,'r')
    richSmartIds = fileRichSmartIDs.read().split()
    fileRichSmartIDs.close()
    return richSmartIds

def detectorNumbersFileOpen(filePath):

    myFile = open(filePath,'w')

    myFile.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    myFile.write('<!DOCTYPE DDDB SYSTEM "git:/DTD/structure.dtd">\n\n')
    myFile.write('<DDDB>\n')
    myFile.write('<condition classID="5"  name="PMTDetectorNumbers">\n\n')
    
    return myFile

def detectorNumbersFileClose(myFile):

    myFile.write('</condition>\n')
    myFile.write('</DDDB>\n')

    myFile.close()

def writeCopyNumberList(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    counter = 0

    for el in smartIdHelper.SmartId.validCopyNumbers(numberBegin, numberEnd):
        outputFile.write(str(el)+'\t')
        counter +=1

    outputFile.write('\n</paramVector>\n\n')

    print("Saved  (dummy - valid copy number) list: " + aName + " with #elements: " + str(counter))

def writeModuleList(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    counter = 0

    for el in smartIdHelper.SmartId.validCopyNumbers(numberBegin, numberEnd):
        outputFile.write(str(el // 16)+'\t')
        counter +=1

    outputFile.write('\n</paramVector>\n\n')
 
    print("Saved  (dummy - module from valid copy number) list: " + aName + " with #elements: " + str(counter))

def writePmtInModuleList(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    counter = 0

    for el in smartIdHelper.SmartId.validCopyNumbers(numberBegin, numberEnd):
        outputFile.write(str(el % 16)+'\t')
        counter +=1

    outputFile.write('\n</paramVector>\n\n')

    print("Saved  (dummy - pmt number in module from valid copy number) list: " + aName + " with #elements: " + str(counter))

def writeModuleMap(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    counter = 0

    for el in smartIdHelper.SmartId.validCopyNumbers(numberBegin, numberEnd):
        outputFile.write("{0}/{1}\t".format(str(el // 16),str(el // 16)))
        counter +=1

    outputFile.write('\n</paramVector>\n\n')

    print("Saved  (dummy - module from valid copy number) map: " + aName + " with #elements: " + str(counter))

def writeRichSmartIdList(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    counter = 0
    
    richSmartIDList = readRichSmartIds(smartIdsInputPath)

    for el in smartIdHelper.SmartId.validCopyNumbers(numberBegin, numberEnd):
        outputFile.write(str(richSmartIDList[el])+'\t')
        counter +=1

    outputFile.write('\n</paramVector>\n\n')

    print("Saved valid RichSmartID list: " + aName + " with #elements: " + str(counter))

def writeEmptyList(outputFile, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))

    counter = 0

    outputFile.write('\n</paramVector>\n\n')

    print("Saved empty list: " + aName + " with #elements: " + str(counter))

####################################################################

import os
os.mkdir(outputPath)
os.mkdir(outputPath + "/Rich1")
os.mkdir(outputPath + "/Rich2")
os.mkdir(outputPathRich1)
os.mkdir(outputPathRich2)

#RICH1
rich1File = detectorNumbersFileOpen(outputPathRich1+'/PMTDetectorNumbers.xml')

rich1File.write('<param name="NumberOfPMTs" type="int" comment="Number of active PMTs">' + str(smartIdHelper.nPmtsR1) + '</param>\n\n')

print("\n---> Creating lists for Rich1:\n")

for l in listsToCreateCopyNumber:
    writeCopyNumberList(rich1File, 0, nMaxPmtCopyNumberRich1, l[0], l[1], l[2])
for l in listsToCreateRichSmartId:
    writeRichSmartIdList(rich1File, 0, nMaxPmtCopyNumberRich1, l[0], l[1], l[2])
for l in listsToCreateModuleNumber:
    writeModuleList(rich1File, 0, nMaxPmtCopyNumberRich1, l[0], l[1], l[2])
for l in listsToCreatePmtInModuleNumber:
    writePmtInModuleList(rich1File, 0, nMaxPmtCopyNumberRich1, l[0], l[1], l[2])
for l in mapsToCreate:
    writeModuleMap(rich1File, 0, nMaxPmtCopyNumberRich1, l[0], l[1], l[2])
for l in listsToCreateEmpty:
    writeEmptyList(rich1File,l[0], l[1], l[2])

detectorNumbersFileClose(rich1File)

print("\n<--- Done for Rich1\n\n")

#RICH2

rich2File = detectorNumbersFileOpen(outputPathRich2+'/PMTDetectorNumbers.xml')

rich2File.write('<param name="NumberOfPMTs" type="int" comment="Number of active PMTs">' + str(smartIdHelper.nPmtsR2) + '</param>\n\n')

print("\n---> Creating lists for Rich2:\n")

for l in listsToCreateCopyNumber:
    writeCopyNumberList(rich2File, nMaxPmtCopyNumberRich1, nMaxPmtCopyNumberRich1+nMaxPmtCopyNumberRich2, l[0], l[1], l[2])
for l in listsToCreateRichSmartId:
    writeRichSmartIdList(rich2File, nMaxPmtCopyNumberRich1, nMaxPmtCopyNumberRich1+nMaxPmtCopyNumberRich2, l[0], l[1], l[2])
for l in listsToCreateModuleNumber:
    writeModuleList(rich2File, nMaxPmtCopyNumberRich1, nMaxPmtCopyNumberRich1+nMaxPmtCopyNumberRich2, l[0], l[1], l[2])
for l in listsToCreatePmtInModuleNumber:
    writePmtInModuleList(rich2File, nMaxPmtCopyNumberRich1, nMaxPmtCopyNumberRich1+nMaxPmtCopyNumberRich2, l[0], l[1], l[2])
for l in mapsToCreate:
    writeModuleMap(rich2File, nMaxPmtCopyNumberRich1, nMaxPmtCopyNumberRich1+nMaxPmtCopyNumberRich2, l[0], l[1], l[2])
for l in listsToCreateEmpty:
    writeEmptyList(rich2File,l[0], l[1], l[2])

detectorNumbersFileClose(rich2File)

print("\n<--- Done for Rich2\n\n")
