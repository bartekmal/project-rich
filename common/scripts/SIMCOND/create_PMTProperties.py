#! /usr/bin/env python2

import os

#consts

nrOfChannelsInPmt = 64
nModulesRich1 = 132
nMaxPmtCopyNumberRich1 = 2112
nPmtsRich1 = 1888
nModulesRich2 = 144
nPmtsRich2 = 1152
nMaxPmtCopyNumberRich2 = 2304

# ! hack (diffs in the list read from a file and the actual number of valid PMTs) ('calculated' manually)
diffInValidPmts = list(range(0,8)) + list(range(72,80)) + list(range(960, 968)) + list(range(1032,1040))

#functions

def readAListFromFile(filePath, separator = ''):
    inputFile = open(filePath,'r')
    listFromFile = inputFile.read().split( separator )
    inputFile.close()
    return listFromFile


def sanitiseListFromFile(curList, indicesToRemove):

    # ! remove from the end (to avoid change in indices)
    for el in sorted(indicesToRemove, reverse = True):
        del curList[el]

    return curList

def detectorNumbersFileOpen( filePath, catalogName="" ):

    myFile = open(filePath,'w')

    myFile.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    myFile.write('<!DOCTYPE DDDB SYSTEM "git:/DTD/structure.dtd">\n\n')
    myFile.write('<DDDB>\n\n')
    if not catalogName=="":
        myFile.write('  <catalog name="{0}">\n\n'.format( catalogName ))
    
    return myFile

def detectorNumbersFileClose(myFile, catalogName="" ):

    if not catalogName=="":
        myFile.write('\n  </catalog>\n\n')
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
            print("I have a bad feeling about this...")

def createValidCopyNumberList(copyNumberBegin, copyNumberEnd):
    
    copyNumberList = []

    noEC0TypeModules =  list(range(6,66,6))+list(range(72,132,6))
    noEC3TypeModules =  list(range(11,71,6))+list(range(77,137,6))

    emptyModules = [0, 5, 66, 71]

    for pmtCopyNumber in range(copyNumberBegin,copyNumberEnd):
        moduleGlobal = int(pmtCopyNumber//16)
        pmtInModule = pmtCopyNumber % 16

        #RICH1
        if pmtCopyNumber < nMaxPmtCopyNumberRich1:

            #empty (corner) modules
            if moduleGlobal in emptyModules:
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

def parameterValueForPixel( meanValue ):
    return meanValue

def createVectorParam( outputFile, paramVect ):

    outputFile.write( '      <paramVector name="{0}" type="{1}" comment="{2}">\n'.format( paramVect[0], paramVect[1], paramVect[2] ) )

    outputFile.write( '        ')
    for i in range( nrOfChannelsInPmt ):
        outputFile.write( str( parameterValueForPixel( paramVect[3] ) )+'\t' )
    outputFile.write( '\n')

    outputFile.write( '      </paramVector>\n' )
    outputFile.write( '\n' )

def createSINVectorParam( outputFile, occupancy, moduleNrGlobal, countersNrOfPmts ):

    occupancy = float(occupancy)
    
    #find occupancy region
    currentPMTOccupancyRegion = -1
    if occupancy > 20.0:
        currentPMTOccupancyRegion = 0
        countersNrOfPmts[0] = countersNrOfPmts[0] + 1
    elif occupancy > 15.0:
        currentPMTOccupancyRegion = 1
        countersNrOfPmts[1] = countersNrOfPmts[1] + 1
    elif occupancy > 8.0:
        currentPMTOccupancyRegion = 2
        countersNrOfPmts[2] = countersNrOfPmts[2] + 1
    elif occupancy > 2.0:
        currentPMTOccupancyRegion = 3
        countersNrOfPmts[3] = countersNrOfPmts[3] + 1
    elif occupancy > 1.0:
        currentPMTOccupancyRegion = 4
        countersNrOfPmts[4] = countersNrOfPmts[4] + 1
    else:
        currentPMTOccupancyRegion = 5
        countersNrOfPmts[5] = countersNrOfPmts[5] + 1

    sinInputFiles = [ "input/sinProb/900V/output/sinRatio_{0}.txt".format( occupancyRegion ) for occupancyRegion in range(1,8) ]
    sinProbabilityList = readAListFromFile( sinInputFiles[ currentPMTOccupancyRegion ], "," )

    #create the paramVector
    outputFile.write(
        '      <paramVector name="SINRatio" type="double" comment="SIN B/S ratio for each channel (pixel) in the PMT (values measured with HV = 900 V).">\n')

    outputFile.write( '        ')
    
    if isGrandModule( moduleNrGlobal ):
        for i in range( nrOfChannelsInPmt ):
            outputFile.write( '0.0\t' )
    else:
        for i in range( nrOfChannelsInPmt ):
            outputFile.write( str( float(sinProbabilityList[i]) )+'\t' )
    
    outputFile.write( '\n')

    outputFile.write( '      </paramVector>\n' )
    outputFile.write( '\n' )

    return countersNrOfPmts

def createConditionForEachValidCopyNumber( outputFile, numberBegin, numberEnd, conditionName, paramListConstantValue, paramVectListConstantValue, paramListFromFile ):

    validCopyNumbersList = createValidCopyNumberList(numberBegin, numberEnd)

    counter = 0
    countersNrOfPmts = [ 0, 0, 0, 0, 0, 0 ]

    #create a condition for each PMT
    for i_el in range( len( validCopyNumbersList ) ):
        outputFile.write('    <condition classID="5"  name="PMT{0}_{1}">\n'.format(validCopyNumbersList[i_el], conditionName))
        outputFile.write('\n')

        moduleNrGlobal = validCopyNumbersList[i_el] // 16

        #create parameters
        for param in paramListConstantValue:
            if param[1] == 'int':
                outputFile.write( '      <param name="{0}" type="{1}" comment="{2}"> {3} </param>\n'.format( param[0], param[1], param[2], param[3] ) )
            else: # ! assumes float
                outputFile.write( '      <param name="{0}" type="{1}" comment="{2}"> {3:.3f} </param>\n'.format( param[0], param[1], param[2], param[3] ) )
            outputFile.write( '\n')
        for paramVect in paramVectListConstantValue:
            createVectorParam( outputFile, paramVect )
        for param in paramListFromFile:
            occupancy = param[4][i_el]
            # use previous occupancy for the PMT population wrt SIN properties and occupancy
            occupancyForSin = param[3][i_el]
            outputFile.write( '      <param name="{0}" type="{1}" comment="{2}"> {3:.8f} </param>\n'.format( param[0], param[1], param[2], float(occupancy) / 100. ) )
            outputFile.write( '\n')
            countersNrOfPmts = createSINVectorParam( outputFile, occupancyForSin, moduleNrGlobal, countersNrOfPmts )

        outputFile.write('    </condition>\n')
        outputFile.write('\n')

        counter +=1

    print("Created a condition for each element.\n\tname: {0}\n\t#elements: {1}".format(
        conditionName, len(validCopyNumbersList)))
    print("Number of PMTs in different occupancy regions (for SIN mostly):\n")
    print(countersNrOfPmts)

def createCatalogOfConditions( filePath, numberBegin, numberEnd, catalogName  ):

    listOfValidCopyNumbers = createValidCopyNumberList(numberBegin, numberEnd)

    outputFileName = "ChannelInfoCatalog.xml"
    outputFile = detectorNumbersFileOpen( filePath+'/'+outputFileName, catalogName )

    for el in listOfValidCopyNumbers:
        outputFile.write('    <conditionref href="ChannelInfo/PMTProperties.xml#PMT{0}_Properties"/>\n'.format(el) )

    detectorNumbersFileClose( outputFile, catalogName )
    

####################################################################



outputCatalogName = "ChannelInfo"
catalogName = "PMTProperties"
outputFileName = "PMTProperties.xml"

outputPath = "output"
os.mkdir(outputPath)
os.mkdir(outputPath + "/Rich1")
os.mkdir(outputPath + "/Rich2")
outputPathRich1 = outputPath + "/Rich1/"+outputCatalogName
outputPathRich2 = outputPath + "/Rich2/"+outputCatalogName
os.mkdir(outputPathRich1)
os.mkdir(outputPathRich2)

paramListConstantValue = [
        [ "QEType", "int", "Flag QE shape for the given PD type/series (dummy value for now).", 0 ],
        [ "QEScalingFactor", "double", "QE scaling factor for the given PD (dummy value for now).", 1.000 ]
    ]

paramVectListConstantValue = [
        [ "GainMean", "double", "Gain mean values for each channel (pixel) in the PMT [in millions of electrons].", 0.9 ],
        [ "GainRms", "double", "Gain RMS values for each channel (pixel) in the PMT [in millions of electrons].", 0.2 ],
        [ "Threshold", "double", "Threshold values for each channel (pixel) in the PMT [in millions of electrons].", 0.125 ]
    ]

paramListFromFile_R1 = [
    ["AverageOccupancy", "double", "Average PMT occupancy [probability in range 0-1]. This is the MB occupancy simulated for nu=7.6 with Gauss/v55r1 after the detector plane positions updates done in 2021-07.",
        sanitiseListFromFile(readAListFromFile("input/occupancy/Gauss_v53r2/output/365_percent.txt", ","), diffInValidPmts), sanitiseListFromFile(readAListFromFile("input/occupancy/Gauss_v55r1/detPlanePositions/stdNu_7c6/minBias/output/365_percent.txt", ","), diffInValidPmts)],
]

paramListFromFile_R2 = [
    ["AverageOccupancy", "double", "Average PMT occupancy [probability in range 0-1]. This is the MB occupancy simulated for nu=7.6 with Gauss/v55r1 after the detector plane positions updates done in 2021-07.",
        readAListFromFile("input/occupancy/Gauss_v53r2/output/385_percent.txt", ","), readAListFromFile("input/occupancy/Gauss_v55r1/detPlanePositions/stdNu_7c6/minBias/output/385_percent.txt", ",")],
]


#RICH1

createCatalogOfConditions( outputPath+'/Rich1', 0, nMaxPmtCopyNumberRich1, "Rich1"  )

rich1File = detectorNumbersFileOpen( outputPathRich1+'/'+outputFileName, catalogName )

print("\n---> Creating lists for Rich1:\n")

conditionName = "Properties"
createConditionForEachValidCopyNumber(rich1File, 0, nMaxPmtCopyNumberRich1, conditionName, paramListConstantValue, paramVectListConstantValue, paramListFromFile_R1 )

detectorNumbersFileClose(rich1File, catalogName )

print("\n<--- Done for Rich1\n\n")

#RICH2

createCatalogOfConditions( outputPath+'/Rich2', nMaxPmtCopyNumberRich1, nMaxPmtCopyNumberRich1+nMaxPmtCopyNumberRich2, "Rich2"  )

rich2File = detectorNumbersFileOpen( outputPathRich2+'/'+outputFileName, catalogName )

print("\n---> Creating lists for Rich2:\n")

createConditionForEachValidCopyNumber(rich2File, nMaxPmtCopyNumberRich1, nMaxPmtCopyNumberRich1+nMaxPmtCopyNumberRich2, conditionName, paramListConstantValue, paramVectListConstantValue, paramListFromFile_R2 )

detectorNumbersFileClose( rich2File, catalogName )

print("\n<--- Done for Rich2\n\n")
