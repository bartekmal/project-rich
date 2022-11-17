#!/usr/bin/env python3

'''
Script to update the channel properties in SIMCOND using input from txt files containing the commissioning data.
'''

# Futures
# from __future__ import print_function

# Built-in/Generic Imports
import os
import shutil

from typing import List
from dataclasses import dataclass, field

from importlib.machinery import SourceFileLoader
smartIdHelper = SourceFileLoader('smartId', os.path.join(os.environ['RICH_BASE_SCRIPTS'], 'utils/smartId.py')).load_module()

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

# ! arbitrary values for sanitising unexpected input (may need tuning when input changes)
neutralValForNegativeInputs = f'{float(0.0):5.3f}'
neutralValForLargeInputs = f'{float(0.0):5.3f}'
maxValues900V = [5.0, 5.0, 30.0, 5.0]
maxValues1000V = [10.0, 10.0, 30.0, 10.0]

nEntriesPerCondition = 7
idPatternStart = 'RICH'

conditionsIndentLvl = 2
conditionElements = {
    'qeType': {'name': 'QEType', 'type': 'int', 'comment': 'Flag QE shape for the given PD type/series (dummy value for now).', 'default': 0},
    'qeScalingFactor': {'name': 'QEScalingFactor', 'type': 'double', 'comment': 'QE scaling factor for the given PD (dummy value for now).', 'default': 1.0},
    'gainMeans': {'name': 'GainMean', 'type': 'double', 'comment': 'Gain mean values for each channel (pixel) in the PD [in millions of electrons].', 'default': 0.9},
    'gainStdDevs': {'name': 'GainRms', 'type': 'double', 'comment': 'Gain RMS values for each channel (pixel) in the PD [in millions of electrons].', 'default': 0.2},
    'thresholds': {'name': 'Threshold', 'type': 'double', 'comment': 'Threshold values for each channel (pixel) in the PD [in millions of electrons].', 'default': 0.125},
    'occupancy': {'name': 'AverageOccupancy', 'type': 'double', 'comment': 'Average PD occupancy [probability in range 0-1]. This is the MB occupancy simulated for nu=7.6 with Gauss/v55r4 after the numbering scheme updates in https://gitlab.cern.ch/lhcb-conddb/DDDB/-/merge_requests/107.'},
    'sinMuValues': {'name': 'SINRatio', 'type': 'double', 'comment': 'SIN B/S ratio for each channel (pixel) in the PD (values measured with HV = 900 V).'},
}

# helpers xml


def writeXmlFileBegin(outputFile, catalogName, indent = xmlIndent):
    outputFile.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    outputFile.write('<!DOCTYPE DDDB SYSTEM "git:/DTD/structure.dtd">\n\n')
    outputFile.write('<DDDB>\n\n')
    outputFile.write(f'{indent}<catalog name="{catalogName}">\n\n')


def writeXmlFileEnd(outputFile, indent = xmlIndent):
    outputFile.write(f'\n{indent}</catalog>\n\n')
    outputFile.write('</DDDB>\n')


def xmlParamVector(entryConfig, paramsList):
    return f'<paramVector name="{entryConfig["name"]}" type="{entryConfig["type"]}" comment="{entryConfig["comment"]}"> {" ".join(f"{float(el):5.3f}" for el in paramsList)} </paramVector>'


def xmlParam(entryConfig, param):
    return f'<param name="{entryConfig["name"]}" type="{entryConfig["type"]}" comment="{entryConfig["comment"]}"> {param} </param>'


# helper classes
@dataclass
class ConfigEntry:
    '''Configuration helper class for conditions.'''
    inputProperties: str = ''
    inputOccupancy: str = ''
    output: str = ''
    catalogName: str = ''
    outputCatalogPath: str = ''
    catalogCatalogName: str = ''
    nExpectedConditions: int = 0
    expectedCopyNumbers: List[int] = field(default_factory=List[int])
    maxValues: List[float] = field(default_factory=List[float])
    useDefaultValues: bool = False


@dataclass
class Condition:
    '''Storage class for a PMT condition.'''
    smartId: smartIdHelper.SmartId = smartIdHelper.SmartId()
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
        # TODO improve formatting (? add as arguments in the configuration)

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
# ! hack (diffs in the list of PMTS in a file and the actual number of valid PMTs) ('calculated' manually)
# TODO remove this hack once new input files are available (with list of PMTs consistent with the actually installed/valid ones) (current ones contain lists before the corner modules removal in RICH1)
diffInValidPmts = list(range(0,8)) + list(range(72,80)) + list(range(960, 968)) + list(range(1032,1040))

basePath = os.path.join(os.environ['RICH_BASE_SCRIPTS'], 'SIMCOND')
inputPath = os.path.join(basePath, 'input')
outputPath = os.path.join(basePath, 'output')

catalogName = 'PMTProperties'

configList = {
    # R1
    'R1_900V_sinOrdered': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH1_FAKE_sinOrdered_HV_900.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r4/numberSchemeFinal/stdNu_7c6/minBias/output/365_percent.txt'), os.path.join(outputPath, 'Rich1/ChannelInfo/PMTProperties.xml'), catalogName, os.path.join(outputPath, 'Rich1/ChannelInfoCatalog.xml'), 'Rich1', smartIdHelper.nPmtsR1, smartIdHelper.SmartId.validCopyNumbers(0, smartIdHelper.pmtCopyNumberMaxR1),maxValues900V,useDefaultValues=True),
    # 'R1_1000V_sinOrdered': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH1_FAKE_sinOrdered_HV_1000.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r4/numberSchemeFinal/stdNu_7c6/minBias/output/365_percent.txt'), os.path.join(outputPath, 'Rich1/ChannelInfo/PMTProperties.xml'), catalogName, os.path.join(outputPath, 'Rich1/ChannelInfoCatalog.xml'), 'Rich1', smartIdHelper.nPmtsR1, smartIdHelper.SmartId.validCopyNumbers(0, smartIdHelper.pmtCopyNumberMaxR1), maxValues1000V,useDefaultValues=True),
    # 'R1_900V_sinRandom': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH1_FAKE_sinRandom_HV_900.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r4/numberSchemeFinal/stdNu_7c6/minBias/output/365_percent.txt'), os.path.join(outputPath, 'Rich1/ChannelInfo/PMTProperties.xml'), catalogName, os.path.join(outputPath, 'Rich1/ChannelInfoCatalog.xml'), 'Rich1', smartIdHelper.nPmtsR1, smartIdHelper.SmartId.validCopyNumbers(0, smartIdHelper.pmtCopyNumberMaxR1),maxValues900V,useDefaultValues=True),
    # 'R1_1000V_sinRandom': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH1_FAKE_sinRandom_HV_1000.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r4/numberSchemeFinal/stdNu_7c6/minBias/output/365_percent.txt'), os.path.join(outputPath, 'Rich1/ChannelInfo/PMTProperties.xml'), catalogName, os.path.join(outputPath, 'Rich1/ChannelInfoCatalog.xml'), 'Rich1', smartIdHelper.nPmtsR1, smartIdHelper.SmartId.validCopyNumbers(0, smartIdHelper.pmtCopyNumberMaxR1),maxValues1000V,useDefaultValues=True),
    # R2
    'R2_900V': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH2_HV_900.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r4/numberSchemeFinal/stdNu_7c6/minBias/output/385_percent.txt'), os.path.join(outputPath, 'Rich2/ChannelInfo/PMTProperties.xml'), catalogName, os.path.join(outputPath, 'Rich2/ChannelInfoCatalog.xml'), 'Rich2', smartIdHelper.nPmtsR2, smartIdHelper.SmartId.validCopyNumbers(smartIdHelper.pmtCopyNumberMaxR1, smartIdHelper.pmtCopyNumberMaxR1 + smartIdHelper.pmtCopyNumberMaxR2), maxValues900V,useDefaultValues=True),
    # 'R2_1000V': ConfigEntry(os.path.join(inputPath, 'channelProperties/RICH2_HV_1000.txt'), os.path.join(inputPath, 'occupancy/Gauss_v55r4/numberSchemeFinal/stdNu_7c6/minBias/output/385_percent.txt'), os.path.join(outputPath, 'Rich2/ChannelInfo/PMTProperties.xml'), catalogName, os.path.join(outputPath, 'Rich2/ChannelInfoCatalog.xml'), 'Rich2', smartIdHelper.nPmtsR2, smartIdHelper.SmartId.validCopyNumbers(smartIdHelper.pmtCopyNumberMaxR1, smartIdHelper.pmtCopyNumberMaxR1 + smartIdHelper.pmtCopyNumberMaxR2), maxValues1000V,useDefaultValues=True),
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

def sanitiseList(curList, indicesToRemove):

    # ! remove from the end (to avoid change in indices)
    for el in sorted(indicesToRemove, reverse = True):
        del curList[el]

    return curList

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
    isFormatOk = isListExpectedSize(gainMeans, smartIdHelper.nChannelsPmt) and isListExpectedSize(
        gainStdDevs, smartIdHelper.nChannelsPmt) and isListExpectedSize(thresholds, smartIdHelper.nChannelsPmt) and isListExpectedSize(sinMuValues, smartIdHelper.nChannelsPmt)

    # sanitise negative numbers in input
    # ! needs manual tuning (currently: set to zero)
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
    # ! needs manual tuning (currently: set to zero)
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
        smartId = smartIdHelper.SmartId.initFromIdString(idString)

        # ! check if the obtained copy number matches the expected one
        if not smartId.copyNumber() == expectedCopyNr:
            print(f'Copy number in the input does not match the expected one: {smartId.copyNumber()} VS {expectedCopyNr}\n')
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

    # prevalidate the input file (check if lines are not missing)
    nLinesInput = nLinesInFile(config.inputProperties)
    nLinesExpected = config.nExpectedConditions * nEntriesPerCondition

    # TODO remove this hack once new input files are available (with list of PMTs consistent with the actually installed/valid ones)
    if ( (nLinesInput % nEntriesPerCondition != 0 ) or (nLinesInput < nLinesExpected) ):
        print(f'Ivalid #lines in {config.inputProperties}:')
        print(
            f'\t expected: {nLinesExpected}')
        print(f'\t current: {nLinesInput}')
        return 1

    # get occupancies (and validate)
    # ! assumes that conditions (properties) are listed in an increasing order (of copyNumber / smartId)
    # ! occupancy converted to float in 0-1 range
    occupancyList = [float(el) / 100.0 for el in getListFromFile(
        config.inputOccupancy, separatorForLists)]

    # TODO remove this hack once new input files are available (with list of PMTs consistent with the actually installed/valid ones)
    if len(occupancyList) == config.nExpectedConditions + len(diffInValidPmts):
        print(f'Sanitising occupancy list of initial length {len(occupancyList)} (expected: {config.nExpectedConditions}) in: {config.inputOccupancy}.\n')
        occupancyList = sanitiseList(occupancyList, diffInValidPmts)

    if not len(occupancyList) == config.nExpectedConditions:
        print(f'Ivalid #lines in: {config.inputOccupancy}.\n')
        return 1

    # prepare the output file
    if not os.path.exists(os.path.dirname(config.output)):
        os.makedirs(os.path.dirname(config.output))

    with open(config.output, 'w', encoding='utf8') as outputFile:
        writeXmlFileBegin(outputFile, config.catalogName)

        # read all conditions
        counterValidConditions = 0
        
        occupancyListIter = iter(occupancyList)
        validCopyNumbersIter = iter(config.expectedCopyNumbers)

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
                        # reset iterators and continue
                        # TODO remove this hack once new input files are available (with list of PMTs consistent with the actually installed/valid ones)
                        occupancyListIter = iter(occupancyList[counterValidConditions:])
                        validCopyNumbersIter = iter(config.expectedCopyNumbers[counterValidConditions:])
                    elif status == 0:
                        # process a valid condition
                        counterValidConditions += 1
                        condition.writeToXml(outputFile, conditionsIndentLvl)

        # check #valid conditions
        if not counterValidConditions == config.nExpectedConditions:
            print(f'Invalid #conditions from: {config.inputProperties}\n')
            return 1
        else:
            print(f'\t #valid conditions in file: {counterValidConditions:5d}\n')

        # close the output file
        writeXmlFileEnd(outputFile)

    return 0

def makeConditionsCatalog( filePath, catalogName, copyNumbers ):

    # prepare the output file
    if not os.path.exists(os.path.dirname(filePath)):
        os.makedirs(os.path.dirname(filePath))

    with open(filePath, 'w', encoding='utf8') as outputFile:
        writeXmlFileBegin(outputFile, catalogName, '  ')
        for el in copyNumbers:
            outputFile.write('    <conditionref href="ChannelInfo/PMTProperties.xml#PMT{0}_Properties"/>\n'.format(el) )
        writeXmlFileEnd(outputFile, '  ')

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
        # ! adapt naming schemes in both conditions & conditions catalog if needed
        makeConditionsCatalog(entry.outputCatalogPath, entry.catalogCatalogName, entry.expectedCopyNumbers)
        makeConditions(entry)


if __name__ == '__main__':
    main()
