#! /usr/bin/env python

import re
import os


def getOccupancy(f):

    # load original bin contents
    binContentsOriginal = []
    for line in f:
        m = re.search('SetBinContent\(.+?,(.+?)\);', line)
        if m:
            binContentsOriginal.append(m.group(1))

    # translate to real pixel map
    binContentsFinal = binContentsOriginal

    print("Nr of elements: " + str(len(binContentsFinal)))

    # check
    # for i in range( len( binContentsFinal ) ):
    #    print (str(i) + " : \t " + str(binContentsOriginal[i]) + " \t " +  str(binContentsFinal[i]))

    return binContentsFinal


def processSingleInput(dirName, inputFiles):

    # prepare / open files
    inputPath = os.path.abspath(os.path.join(dirName, "input"))
    outputPath = os.path.abspath(os.path.join(dirName, "output"))
    os.mkdir(outputPath)

    fileNameOutput = [
        "365_percent.txt",
        "385_percent.txt",
    ]

    myFileInput = [
        open(inputPath + "/" + fileName, 'r') for fileName in inputFiles
    ]
    myFileOutput = [
        open(outputPath + "/" + fileName, 'w') for fileName in fileNameOutput
    ]

    # execute
    for f_in, f_out in zip(myFileInput, myFileOutput):
        f_out.write(",".join(str(el) for el in getOccupancy(f_in)))

    # close
    for f in myFileInput:
        f.close()
    for f in myFileOutput:
        f.close()


#############################

for nu in ['stdNu_7c6']:
    for eventType in ['minBias']:
        processSingleInput(
            "Gauss_v55r4/numberSchemeFinal/{}/{}".format(nu, eventType),
            ["occupancyPd_Gauss_R1.C", "occupancyPd_Gauss_R2.C"])
