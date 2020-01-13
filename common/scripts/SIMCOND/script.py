#! /usr/bin/env python

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

def isGrandModule(moduleGlobal):
    if moduleGlobal < nModulesRich1:
        return False
    else:
        if moduleGlobal % 6 in [2, 3]:
            return False
        elif moduleGlobal % 6 in [0, 1, 4, 5]:
            return True
        else:
            print "I have a bad feeling about this..."

def createValidCopyNumberList(copyNumberBegin, copyNumberEnd):
    
    copyNumberList = []

    noEC0TypeModules = range(6,66,6)+range(72,132,6)
    noEC3TypeModules = range(11,71,6)+range(77,137,6)

    for pmtCopyNumber in range(copyNumberBegin,copyNumberEnd):
        moduleGlobal = pmtCopyNumber/16
        pmtInModule = pmtCopyNumber % 16

        #RICH1
        if pmtCopyNumber < nMaxPmtCopyNumberRich1:

            #noEC01TypeModules
            if (moduleGlobal == 0 or moduleGlobal == 66) and pmtInModule < 8:
                continue
                #noEC23TypeModules
            elif (moduleGlobal == 5 or moduleGlobal == 71) and pmtInModule >= 8:
                continue
                #noEC0TypeModules
            elif moduleGlobal in noEC0TypeModules and pmtInModule < 4:
                continue
                #noEC3TypeModules
            elif moduleGlobal in noEC3TypeModules and pmtInModule >= 12:
                continue
            else:
                copyNumberList.append(pmtCopyNumber)
                
        #RICH2
        else:       
            if isGrandModule(moduleGlobal) and pmtInModule >= 4:
                continue
            else:
                copyNumberList.append(pmtCopyNumber)
                
    return copyNumberList

def writeCopyNumberList(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    validCopyNumbersList = createValidCopyNumberList(numberBegin, numberEnd)

    counter = 0

    for el in validCopyNumbersList:
        outputFile.write(str(el)+'\t')
        counter +=1

    outputFile.write('\n</paramVector>\n\n')

    print "Saved  (dummy - valid copy number) list: " + aName + " with #elements: " + str(counter)

def writeModuleList(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    validCopyNumbersList = createValidCopyNumberList(numberBegin, numberEnd)

    counter = 0

    for el in validCopyNumbersList:
        outputFile.write(str(el / 16)+'\t')
        counter +=1

    outputFile.write('\n</paramVector>\n\n')
 
    print "Saved  (dummy - module from valid copy number) list: " + aName + " with #elements: " + str(counter)

def writePmtInModuleList(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    counter = 0

    validCopyNumbersList = createValidCopyNumberList(numberBegin, numberEnd)

    for el in validCopyNumbersList:
        outputFile.write(str(el % 16)+'\t')
        counter +=1

    outputFile.write('\n</paramVector>\n\n')

    print "Saved  (dummy - pmt number in module from valid copy number) list: " + aName + " with #elements: " + str(counter)

def writeModuleMap(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    counter = 0

    validCopyNumbersList = createValidCopyNumberList(numberBegin, numberEnd)

    for el in validCopyNumbersList:
        outputFile.write("{0}/{1}\t".format(str(el / 16),str(el / 16)))
        counter +=1

    outputFile.write('\n</paramVector>\n\n')

    print "Saved  (dummy - module from valid copy number) map: " + aName + " with #elements: " + str(counter)

def writeRichSmartIdList(outputFile, numberBegin, numberEnd, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))
    
    counter = 0
    
    validCopyNumbersList = createValidCopyNumberList(numberBegin, numberEnd)

    richSmartIDList = readRichSmartIds("input/RichSmartIDs.txt")

    for el in validCopyNumbersList:
        outputFile.write(str(richSmartIDList[el])+'\t')
        counter +=1

    outputFile.write('\n</paramVector>\n\n')

    print "Saved valid RichSmartID list: " + aName + " with #elements: " + str(counter)

def writeEmptyList(outputFile, aName, aType, aComment):

    outputFile.write('<paramVector name="{0}" type="{1}" comment="{2}">\n'.format(aName,aType,aComment))

    counter = 0

    outputFile.write('\n</paramVector>\n\n')

    print "Saved empty list: " + aName + " with #elements: " + str(counter)

####################################################################

import os
outputPath = "output"
os.mkdir(outputPath)
os.mkdir(outputPath + "/Rich1")
os.mkdir(outputPath + "/Rich2")
outputPathRich1 = outputPath + "/Rich1/ReadoutConf"
outputPathRich2 = outputPath + "/Rich2/ReadoutConf"
os.mkdir(outputPathRich1)
os.mkdir(outputPathRich2)

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

#config
nModulesRich1 = 132
nMaxPmtCopyNumberRich1 = 2112
nPmtsRich1 = 1920

nModulesRich2 = 144
nPmtsRich2 = 1152
nMaxPmtCopyNumberRich2 = 2304

#RICH1
rich1File = detectorNumbersFileOpen(outputPathRich1+'/PMTDetectorNumbers.xml')

rich1File.write('<param name="NumberOfPMTs" type="int" comment="Number of active PMTs">' + str(nPmtsRich1) + '</param>\n\n')

print "\n---> Creating lists for Rich1:\n"

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

print "\n<--- Done for Rich1\n\n"

#RICH2

rich2File = detectorNumbersFileOpen(outputPathRich2+'/PMTDetectorNumbers.xml')

rich2File.write('<param name="NumberOfPMTs" type="int" comment="Number of active PMTs">' + str(nPmtsRich2) + '</param>\n\n')

print "\n---> Creating lists for Rich2:\n"

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

print "\n<--- Done for Rich2\n\n"
