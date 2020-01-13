#! /usr/bin/env python

import re
import os
import numpy as np
######################################################################3

def getRatesMap(f):

    sinRatesInBins = []
    #load rates as in root map
    for line in f:
        m = re.search('SetBinContent\(..,(.+?)\);', line)
        if m:
            sinRatesInBins.append( m.group(1) )

    #translate to real pixel map
    sinRatesInPixels = []
    for i in range( len( sinRatesInBins )  ):
       sinRatesInPixels.append(0) 

    #set proper values
    pixelsInRowCol = 8
    for i in range( len( sinRatesInBins ) ):
        colInBins = i % pixelsInRowCol
        rowInBins = i // pixelsInRowCol
        colInPixels = colInBins
        rowInPixels = ( pixelsInRowCol - 1 ) - rowInBins
        i_Pixels = rowInPixels * pixelsInRowCol + colInPixels

        #print str(i) + " -> "  + str(i_Pixels)

        sinRatesInPixels[i_Pixels] = sinRatesInBins[i]
    
    #check
    #for i in range( len( sinRatesInPixels ) ):
    #    print str(i) + " : \t " + str(sinRatesInBins[i]) + " \t " +  str(sinRatesInPixels[i])

    #xcheck
    sinRateAverage = sum(list(np.float_(sinRatesInPixels))) / len(sinRatesInPixels)
    print "Expected increase in occupancy for PMT class: {0} is: {1}".format(f, sinRateAverage)

    return sinRatesInPixels

#############################
 

#prepare / open files
inputPath = "input"
outputPath = "output"
os.mkdir(outputPath)

fileNameInput = [ "sinProb_{0}.C".format(i) for i in range(1,8) ]
fileNameOutput = [ "sinProb_{0}.txt".format(i) for i in range(1,8) ]

myFileInput = [ open(inputPath+"/"+fileName,'r') for fileName in fileNameInput ]
myFileOutput = [ open(outputPath+"/"+fileName,'w') for fileName in fileNameOutput ]

#execute
for f_in, f_out in zip( myFileInput,myFileOutput ):
    sinMap = getRatesMap(f_in)
    for el in sinMap:
        f_out.write( str(el) + ", " )
    
#close
for f in myFileInput:
    f.close()
for f in myFileOutput:
    f.close()

