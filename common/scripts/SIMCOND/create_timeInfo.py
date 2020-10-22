#! /usr/bin/env python

import csv
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
from numpy import arange

# config
# tune to the measurement points
inputSignals = [0.000, 0.078, 0.156, 0.234, 0.312, 0.390, 0.468, 0.546, 0.624, 0.702, 0.780, 0.858, 0.936, 1.014, 1.092, 1.170, 1.248, 1.326, 1.404, 1.482, 1.560, 1.638, 1.716, 1.794,
                1.872, 1.950, 2.028, 2.106, 2.184, 2.262, 2.340, 2.418, 2.496, 2.574, 2.652, 2.730, 2.808, 2.886, 2.964, 3.042, 3.120, 3.198, 3.276, 3.354, 3.432, 3.510, 3.588, 3.666, 3.744, 3.822, 3.900]
thresholdValues = [x for x in arange(0.100, 0.500 + 0.025, 0.025)]

# thresholdValues = [0.090, 0.210, 0.450, 0.930] # for xcheck with original width/delay plot


def delayForSingleInputSignal(timeInfo, inputSignal):
    record = timeInfo[inputSignal]
    x = [el[0] for el in record]
    y = [el[1] for el in record]

    return [x, y]


def widthForSingleInputSignal(timeInfo, inputSignal):
    record = timeInfo[inputSignal]
    x = [el[0] for el in record]
    y = [el[2] for el in record]

    return [x, y]


# define data structures
timeInfoDict = {}
funcsDelay = {}
funcsWidth = {}
for inputSignal in inputSignals:
    timeInfoDict[inputSignal] = []

# read input from file
with open('input/timeInfo/widthAndDelay.csv', mode='r') as csv_file:

    csv_reader = csv.DictReader(csv_file)
    line_count = 0
    for row in csv_reader:
        if line_count == 0:
            print('Column names are: ' + "\t".join(row))
            line_count += 1

        timeInfoDict[float(row["inputSignal"])].append([
            float(row["threshold"]),
            float(row["delay"]),
            float(row["width"])
        ])
        line_count += 1

    print('Processed nr of lines: {}'.format(line_count))

# create plots
figDelay, axDelay = plt.subplots(1)
figWidth, axWidth = plt.subplots(1)

for inputSignal in inputSignals:

    delay = delayForSingleInputSignal(timeInfoDict, inputSignal)
    width = widthForSingleInputSignal(timeInfoDict, inputSignal)

    if len(delay[0]) > 1 and len(width[0]) > 1:

        funcDelay = interp1d(
            delay[0], delay[1], kind='linear', bounds_error=False)
        funcWidth = interp1d(
            width[0], width[1], kind='linear', bounds_error=False)

        funcsDelay[inputSignal] = funcDelay
        funcsWidth[inputSignal] = funcWidth

        axDelay.plot(
            delay[0],
            delay[1],
            'o',
            thresholdValues,
            funcDelay(thresholdValues),
            '--',
            label=str(inputSignal))

        axWidth.plot(
            width[0],
            width[1],
            'o',
            thresholdValues,
            funcWidth(thresholdValues),
            '--',
            label=str(inputSignal))

    else:
        axDelay.plot(delay[0], delay[1], 'o', label=str(inputSignal))
        axWidth.plot(width[0], width[1], 'o', label=str(inputSignal))

# save plots
plt.legend(loc='best')
figDelay.savefig('output/timeInfo/delayVSthreshold.png')
figWidth.savefig('output/timeInfo/widthVSthreshold.png')

# create time info in proper format
outputFileWidth = open('output/timeInfo/width.dat', 'w')
outputFileDelay = open('output/timeInfo/delay.dat', 'w')

figDelayInThreshold, axDelayInThreshold = plt.subplots(1)
figWidthInThreshold, axWidthInThreshold = plt.subplots(1)

for threshold in thresholdValues:
    delayInThreshold = []
    widthInThreshold = []
    for inputSignal in inputSignals:
        if inputSignal in funcsDelay:
            delayInThreshold.append([
                float(inputSignal),
                float(funcsDelay[inputSignal](threshold))
            ])
        else:
            pass
            #delay = delayForSingleInputSignal(timeInfoDict, inputSignal)
            #delayInThreshold.append([float(inputSignal), float(delay[1][0])])

        if inputSignal in funcsWidth:
            widthInThreshold.append([
                float(inputSignal),
                float(funcsWidth[inputSignal](threshold))
            ])
        else:
            pass
            #width = widthForSingleInputSignal(timeInfoDict, inputSignal)
            #widthInThreshold.append([float(inputSignal), float(width[1][0])])

    # final plots
    axDelayInThreshold.plot([el[0] for el in delayInThreshold],
                            [el[1] for el in delayInThreshold],
                            'o-',
                            label=str(threshold))
    axDelayInThreshold.set_xscale('log')
    axDelayInThreshold.set_ylim([0, 45])
    plt.grid(True)

    axWidthInThreshold.plot([el[0] for el in widthInThreshold],
                            [el[1] for el in widthInThreshold],
                            'o-',
                            label=str(threshold))
    axWidthInThreshold.set_xscale('log')
    axWidthInThreshold.set_ylim([0, 45])
    plt.grid(True)

    # print to file
    outputFileDelay.write("% threshold = {:.3f}\n".format(threshold))
    for el in delayInThreshold:
        outputFileDelay.write("{:5.3f}\t{:7.4f}\n".format(el[0], el[1]))
    outputFileDelay.write("\n")

    outputFileWidth.write("% threshold = {:.3f}\n".format(threshold))
    for el in widthInThreshold:
        outputFileWidth.write("{:5.3f}\t{:7.4f}\n".format(el[0], el[1]))
    outputFileWidth.write("\n")

# save plots
plt.legend(loc='best')
figDelayInThreshold.savefig('output/timeInfo/delayVSinputSignal.png')
figWidthInThreshold.savefig('output/timeInfo/widthVSinputSignal.png')

# cleanup
outputFileDelay.close()
outputFileWidth.close()
