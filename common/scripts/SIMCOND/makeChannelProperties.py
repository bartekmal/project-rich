#!/usr/bin/env python

'''
Script to update the channel properties in SIMCOND using input from txt files containing the commissioning data.
'''

# Futures
# from __future__ import print_function

# Built-in/Generic Imports
import os
import shutil
import math

from typing import List
from dataclasses import dataclass, field

from parse import parse

# Libs
# import ROOT
# ROOT.gROOT.SetBatch(True)

# Own modules
# from {path} import {class}

__author__ = 'Bartosz Malecki'
__email__ = 'bartosz.piotr.malecki@cern.ch'

# global variables
separatorForLists = ','
newlineSymbol = '\n'
xmlIndent = '\t'

neutralValForNegativeInputs = f'{float(0.0):5.3f}'
neutralValForLargeInputs = f'{float(0.0):5.3f}'
maxValues900V = [5.0, 5.0, 30.0, 5.0]
maxValues1000V = [10.0, 10.0, 30.0, 10.0]

nModulesColumn = 6
nEcsModule = 4
nPmtsEc = 4
nPmtsModule = nEcsModule * nPmtsEc
nChannelsPmt = 64

nModulesR1 = 132
nPmtsR1 = 1920
pmtCopyNumberMaxR1 = 2112
nModulesR2 = 144
nPmtsR2 = 1152
pmtCopyNumberMaxR2 = 2304

moduleGlobalStart = {
    'R1U': 0,
    'R1D': math.floor(nModulesR1/2),
    'R2A': nModulesR1,
    'R2C': nModulesR1 + math.floor(nModulesR2/2)
}

nEntriesPerCondition = 7
idPattern = '{}/{}/{}_COL{}/PDM{}/EC{}/PMT{}'
idPatternStart = 'RICH'

basePath = '/afs/cern.ch/work/b/bmalecki/RICH_Upgrade/common/scripts/SIMCOND'
inputPath = os.path.join(basePath, 'input')
outputPath = os.path.join(basePath, 'output')

catalogName = 'PMTProperties'
conditionsIndentLvl = 2
conditionElements = {
    'qeType': {'name': 'QEType', 'type': 'int', 'comment': 'Flag QE shape for the given PD type/series (dummy value for now).', 'default': 0},
    'qeScalingFactor': {'name': 'QEScalingFactor', 'type': 'double', 'comment': 'QE scaling factor for the given PD (dummy value for now).', 'default': 1.0},
    'gainMeans': {'name': 'GainMean', 'type': 'double', 'comment': 'Gain mean values for each channel (pixel) in the PD [in millions of electrons].', 'default': 0.9},
    'gainStdDevs': {'name': 'GainRms', 'type': 'double', 'comment': 'Gain RMS values for each channel (pixel) in the PD [in millions of electrons].', 'default': 0.2},
    'thresholds': {'name': 'Threshold', 'type': 'double', 'comment': 'Threshold values for each channel (pixel) in the PD [in millions of electrons].', 'default': 0.125},
    'occupancy': {'name': 'AverageOccupancy', 'type': 'double', 'comment': 'Average PD occupancy [probability in range 0-1]. This is the MB occupancy simulated for nu=7.6 with Gauss/v55r1 after the detector plane positions updates done in 2021-07.'},
    'sinMuValues': {'name': 'SINRatio', 'type': 'double', 'comment': 'SIN B/S ratio for each channel (pixel) in the PD (values measured with HV = 900 V).'},
}

# helpers xml


def writeXmlFileBegin(outputFile, catalogName):
    outputFile.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    outputFile.write('<!DOCTYPE DDDB SYSTEM "git:/DTD/structure.dtd">\n\n')
    outputFile.write('<DDDB>\n\n')
    outputFile.write(f'{xmlIndent}<catalog name="{catalogName}">\n\n')


def writeXmlFileEnd(outputFile):
    outputFile.write(f'\n{xmlIndent}</catalog>\n\n')
    outputFile.write('</DDDB>\n')


def xmlParamVector(entryConfig, paramsList):
    return f'<paramVector name="{entryConfig["name"]}" type="{entryConfig["type"]}" comment="{entryConfig["comment"]}"> {" ".join(f"{float(el):5.3f}" for el in paramsList)} </paramVector>'


def xmlParam(entryConfig, param):
    return f'<param name="{entryConfig["name"]}" type="{entryConfig["type"]}" comment="{entryConfig["comment"]}"> {param} </param>'


# helper classes

@dataclass
class SmartId:
    '''SmartId class'''
    rich: str = ''
    plane: str = ''
    col: int = 0
    module: int = 0
    ec: int = 0
    pmt: int = 0

    @classmethod
    def initFromIdString(cls, idString):
        # ! could be improved (reg expressions?)
        parsedId = parse(idPattern, idString)

        # ! remember that column numbering scheme in RICH2 was changed (here: keep old one for now)
        return cls(parsedId[0], parsedId[1], (int(parsedId[3]) - 1 if parsedId[0] == 'RICH2' else int(parsedId[3])),
                   int(parsedId[4]), int(parsedId[5]), int(parsedId[6]))

    def moduleGlobal(self) -> int:
        return moduleGlobalStart[self.plane] + self.col * nModulesColumn + self.module

    def pmtInModule(self) -> int:
        return (self.ec) if (SmartId.isLargePmt(self.moduleGlobal())) else (self.ec * nPmtsEc + self.pmt)

    def copyNumber(self) -> int:
        '''Return PMT copy number (unique in the whole RICH system).'''
        return self.moduleGlobal() * nPmtsModule + self.pmtInModule()

    @staticmethod
    def isLargePmt(moduleGlobal):
        '''Return True for H-type PMTs/modules.'''
        if moduleGlobal < nModulesR1:
            return False
        else:
            if moduleGlobal % 6 in [2, 3]:
                return False
            elif moduleGlobal % 6 in [0, 1, 4, 5]:
                return True
            else:
                print("I have a bad feeling about this...")
                return None

    @staticmethod
    def validCopyNumbers(copyNumberBegin, copyNumberEnd):
        '''Return a list of valid copy numbers in the given range.'''

        copyNumberList = []

        noEC0TypeModules = list(range(6, 66, 6))+list(range(72, 132, 6))
        noEC3TypeModules = list(range(11, 71, 6))+list(range(77, 137, 6))

        for pmtCopyNumber in range(copyNumberBegin, copyNumberEnd):
            moduleGlobal = int(pmtCopyNumber//16)
            pmtInModule = pmtCopyNumber % 16

            # RICH1
            if pmtCopyNumber < pmtCopyNumberMaxR1:

                # noEC01TypeModules
                if (moduleGlobal == 0 or moduleGlobal == 66) and pmtInModule < 8:
                    continue
                    # noEC23TypeModules
                elif (moduleGlobal == 5 or moduleGlobal == 71) and pmtInModule >= 8:
                    continue
                    # noEC0TypeModules
                elif moduleGlobal in noEC0TypeModules and pmtInModule < 4:
                    continue
                    # noEC3TypeModules
                elif moduleGlobal in noEC3TypeModules and pmtInModule >= 12:
                    continue
                else:
                    copyNumberList.append(pmtCopyNumber)

            # RICH2
            else:
                if SmartId.isLargePmt(moduleGlobal) and pmtInModule >= 4:
                    continue
                else:
                    copyNumberList.append(pmtCopyNumber)

        return copyNumberList


@dataclass
class ConfigEntry:
    '''Configuration helper class for conditions.'''
    inputProperties: str = ''
    inputOccupancy: str = ''
    output: str = ''
    catalogName: str = ''
    nExpectedConditions: int = 0
    expectedCopyNumbers: List[int] = field(default_factory=List[int])
    maxValues: List[float] = field(default_factory=List[float])
    useDefaultValues: bool = False


@dataclass
class Condition:
    '''Storage class for a PMT condition.'''
    smartId: SmartId = SmartId()
    qeType: int = 0
    qeScalingFactor: float = 1.
    gainMeans: List[float] = field(default_factory=List[float])
    gainStdDevs: List[float] = field(default_factory=List[float])
    thresholds: List[float] = field(default_factory=List[float])
    occupancy: float = 0.
    sinMuValues: List[float] = field(default_factory=List[float])

    def writeToXml(self, outputFile, indentLvl):
        '''Save the condition to a xml file'''

        # header
        outputFile.write(
            f'{xmlIndent*indentLvl}<condition classID="5"  name="PMT{self.smartId.copyNumber()}_Properties">\n\n')

        # formatting
        # TODO improve formatting (? add as arguments below)

        # elements
        outputFile.write(
            f'{xmlIndent*(indentLvl+1)}{xmlParam(conditionElements["qeType"],self.qeType)}\n')
        outputFile.write(
            f'{xmlIndent*(indentLvl+1)}{xmlParam(conditionElements["qeScalingFactor"],f"{self.qeScalingFactor:4.3f}")}\n')
        outputFile.write(
            f'{xmlIndent*(indentLvl+1)}{xmlParamVector(conditionElements["gainMeans"],self.gainMeans)}\n')
        outputFile.write(
            f'{xmlIndent*(indentLvl+1)}{xmlParamVector(conditionElements["gainStdDevs"],self.gainStdDevs)}\n')
        outputFile.write(
            f'{xmlIndent*(indentLvl+1)}{xmlParamVector(conditionElements["thresholds"],self.thresholds)}\n')
        outputFile.write(
            f'{xmlIndent*(indentLvl+1)}{xmlParam(conditionElements["occupancy"],f"{self.occupancy:7.6f}")}\n')
        outputFile.write(
            f'{xmlIndent*(indentLvl+1)}{xmlParamVector(conditionElements["sinMuValues"],self.sinMuValues)}\n')

        # close
        outputFile.write(f'\n{xmlIndent*indentLvl}</condition>\n\n')


# config
configList = {
    # R1
    'R1_900V_sinOrdered': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH1_FAKE_sinOrdered_HV_900.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r1/detPlanePositions/stdNu_7c6/minBias/output/365_percent.txt'), os.path.join(outputPath, 'Rich1/ChannelInfo/PMTProperties.xml'), catalogName, nPmtsR1, SmartId.validCopyNumbers(0, pmtCopyNumberMaxR1),maxValues900V,useDefaultValues=True),
    # 'R1_1000V_sinOrdered': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH1_FAKE_sinOrdered_HV_1000.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r1/detPlanePositions/stdNu_7c6/minBias/output/365_percent.txt'), os.path.join(outputPath, 'Rich1/ChannelInfo/PMTProperties.xml'), catalogName, nPmtsR1, SmartId.validCopyNumbers(0, pmtCopyNumberMaxR1), maxValues1000V,useDefaultValues=True),
    # 'R1_900V_sinRandom': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH1_FAKE_sinRandom_HV_900.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r1/detPlanePositions/stdNu_7c6/minBias/output/365_percent.txt'), os.path.join(outputPath, 'Rich1/ChannelInfo/PMTProperties.xml'), catalogName, nPmtsR1, SmartId.validCopyNumbers(0, pmtCopyNumberMaxR1),maxValues900V,useDefaultValues=True),
    # 'R1_1000V_sinRandom': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH1_FAKE_sinRandom_HV_1000.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r1/detPlanePositions/stdNu_7c6/minBias/output/365_percent.txt'), os.path.join(outputPath, 'Rich1/ChannelInfo/PMTProperties.xml'), catalogName, nPmtsR1, SmartId.validCopyNumbers(0, pmtCopyNumberMaxR1),maxValues1000V,useDefaultValues=True),
    # R2
    'R2_900V': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH2_HV_900.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r1/detPlanePositions/stdNu_7c6/minBias/output/385_percent.txt'), os.path.join(outputPath, 'Rich2/ChannelInfo/PMTProperties.xml'), catalogName, nPmtsR2, SmartId.validCopyNumbers(pmtCopyNumberMaxR1, pmtCopyNumberMaxR1 + pmtCopyNumberMaxR2), maxValues900V,useDefaultValues=True),
    # 'R2_1000V': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH2_HV_1000.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r1/detPlanePositions/stdNu_7c6/minBias/output/385_percent.txt'), os.path.join(outputPath, 'Rich2/ChannelInfo/PMTProperties.xml'), catalogName, nPmtsR2, SmartId.validCopyNumbers(pmtCopyNumberMaxR1, pmtCopyNumberMaxR1 + pmtCopyNumberMaxR2), maxValues1000V,useDefaultValues=True),
}


# helpers

def isListExpectedSize(list, expectedSize):
    return (len(list) == expectedSize)


def nLinesInFile(filePath):
    with open(filePath, 'r') as inputFile:
        for count, line in enumerate(inputFile):
            pass
    return count + 1


def getListFromFile(filePath, separator):
    with open(filePath, 'r') as inputFile:
        listFromFile = inputFile.read().split(separator)
    return listFromFile

# functions


def makeCondition(firstLine, fileIter, occupancy, expectedCopyNr, maxValues, useDefaultValues):
    '''Return a valid Condition object (or None)'''
    # read all lines for this condition
    # ! ordering important
    idString = firstLine.rstrip(newlineSymbol)
    series = next(fileIter).rstrip(newlineSymbol)
    blueSensitivityIndex = next(fileIter).rstrip(newlineSymbol)
    gainMeans = next(fileIter).rstrip(newlineSymbol).split(separatorForLists)
    gainStdDevs = next(fileIter).rstrip(newlineSymbol).split(separatorForLists)
    sinMuValues = next(fileIter).rstrip(newlineSymbol).split(separatorForLists)
    thresholds = next(fileIter).rstrip(newlineSymbol).split(separatorForLists)

    # validate input format
    isFormatOk = isListExpectedSize(gainMeans, nChannelsPmt) and isListExpectedSize(
        gainStdDevs, nChannelsPmt) and isListExpectedSize(thresholds, nChannelsPmt) and isListExpectedSize(sinMuValues, nChannelsPmt)

    # sanitise negative numbers in input
    # ! needs a manual tuning (currently: set to zero)
    counterNegativeValues = 0
    listsToCheck = [gainMeans, gainStdDevs, sinMuValues, thresholds]
    for curList in listsToCheck:
        for i in range(len(curList)):
            if float(curList[i]) < 0.0:
                curList[i] = neutralValForNegativeInputs
                counterNegativeValues += 1
    if not counterNegativeValues == 0:
        print(
            f'\t (VERIFY) Found {counterNegativeValues} negative numbers for {idString}. Setting them to zero.')

    # sanitise large numbers in input
    # ! needs a manual tuning (currently: set to zero)
    # TODO improve zip(listsToCheck, maxValues) configuration
    counterLargeValues = 0
    listsToCheck = [gainMeans, gainStdDevs, sinMuValues, thresholds]
    for curList, maxValue in zip(listsToCheck, maxValues):
        for i in range(len(curList)):
            if float(curList[i]) > maxValue:
                curList[i] = neutralValForLargeInputs
                counterLargeValues += 1
    if not counterLargeValues == 0:
        print(
            f'\t (VERIFY) Found {counterLargeValues} large numbers for {idString}. Setting them to zero.')

    # return a valid condition
    if not isFormatOk:
        return None, 1
    else:
        smartId = SmartId.initFromIdString(idString)

        if not smartId.copyNumber() == expectedCopyNr:
            return None, 2
        else:
            # process/modify raw input data if necessary
            if useDefaultValues:
                qeType = int(conditionElements['qeType']['default'])
                qeScalingFactor = float(conditionElements['qeScalingFactor']['default'])
                gainMeans = [ float(conditionElements['gainMeans']['default']) for _ in range(len(gainMeans))]
                gainStdDevs = [ float(conditionElements['gainStdDevs']['default']) for _ in range(len(gainStdDevs))]
                thresholds = [ float(conditionElements['thresholds']['default']) for _ in range(len(thresholds))]

            # return the final condition
            return Condition(smartId, qeType, qeScalingFactor, gainMeans, gainStdDevs, thresholds, occupancy, sinMuValues), 0


def makeConditions(config):
    '''Create conditions from the specified input files.'''

    print(f'\t input for channel properties: \t {config.inputProperties}')
    print(f'\t input for occupancy: \t\t {config.inputOccupancy}')
    print(f'\t output file: \t\t\t {config.output}\n')

    # prevalidate the input file
    if not nLinesInFile(config.inputProperties) == config.nExpectedConditions * nEntriesPerCondition:
        print(f'Ivalid #lines in {config.inputProperties}:')
        print(
            f'\t expected: {config.nExpectedConditions * nEntriesPerCondition}')
        print(f'\t obtained: {nLinesInFile(config.inputProperties)}')
        return 1

    # get occupancies (and validate)
    # ! assumes that conditions (properties) are listed in an increasing order (of copyNumber / smartId)
    # ! occupancy converted to float in 0-1 range
    occupancyList = [float(el) / 100.0 for el in getListFromFile(
        config.inputOccupancy, separatorForLists)]
    if not len(occupancyList) == config.nExpectedConditions:
        print(f'Ivalid #lines in {config.inputOccupancy}:')
        return 1
    occupancyListIter = iter(occupancyList)

    # prepare the output file
    if not os.path.exists(os.path.dirname(config.output)):
        os.makedirs(os.path.dirname(config.output))

    with open(config.output, 'w', encoding='utf8') as outputFile:
        writeXmlFileBegin(outputFile, config.catalogName)

        # read all conditions
        # ! check if the obtained copy numbers match the expected ones
        validCopyNumbersIter = iter(config.expectedCopyNumbers)
        counterConditions = 0
        with open(config.inputProperties, 'r', encoding='utf8') as inputProperties:
            for line in inputProperties:
                if line.startswith(idPatternStart):
                    # get and validate the condition
                    condition, status = makeCondition(
                        line, inputProperties, next(occupancyListIter), next(validCopyNumbersIter), config.maxValues, config.useDefaultValues)
                    if status == 1:
                        print(
                            f'Ivalid input format in {config.inputProperties}\n')
                        return 1
                    elif status == 2:
                        print(
                            f'Ivalid input ordering in {config.inputProperties}\n')
                        return 1

                    # process
                    counterConditions += 1
                    condition.writeToXml(outputFile, conditionsIndentLvl)

        # check #valid conditions
        if not counterConditions == config.nExpectedConditions:
            print(f'Invalid #conditions from: {config.inputProperties}\n')
            return 1
        else:
            print(f'\t #conditions in file: {counterConditions:5d}\n')

        # close the output file
        writeXmlFileEnd(outputFile)

    return 0

# main part


def main():

    print('Hello there!\n')

    print(f'Number of available input files: {len(configList):5d}')
    print(f'Recreating the output area: {outputPath}\n')
    if os.path.exists(outputPath):
        shutil.rmtree(outputPath)
    os.makedirs(outputPath)

    for key, entry in configList.items():
        print(f'Creating conditions for configuration: {key:20s}\n')
        makeConditions(entry)


if __name__ == '__main__':
    main()
