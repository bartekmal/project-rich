#!/usr/bin/env python3
'''
Script to generate all possible PD-level smartIDs in the RICH numbering scheme (has an optional flag for 14-column numbering scheme in RICH2).

Execute within LHCb project to get GaudiPython.
'''

import os
import shutil

from GaudiPython import gbl

# config
outputPath = 'output'
outputFileName = 'RichSmartIDs.txt'

dd4hepScheme = False  # ! flag for 14 columns numbering scheme in RICH2

# global variables
extraColumnsInDd4hepScheme = 4

nPDsInModule = 16
nModulesInColumn = 6

nModulesR1 = 132
nModulesR2 = 144 if not dd4hepScheme else 144 + extraColumnsInDd4hepScheme * nModulesInColumn

modulesInRiches = [nModulesR1, nModulesR1 + nModulesR2]
modulesInPanels = [
    nModulesR1 / 2, nModulesR1, nModulesR1 + nModulesR2 / 2,
    nModulesR1 + nModulesR2
]


# helpers
def rich(moduleGlobal):
    if moduleGlobal < modulesInRiches[0]:
        return 0
    elif moduleGlobal < modulesInRiches[1]:
        return 1
    else:
        return None


def panel(moduleGlobal):
    if moduleGlobal < modulesInPanels[0]:
        return 0
    elif moduleGlobal < modulesInPanels[1]:
        return 1
    elif moduleGlobal < modulesInPanels[2]:
        return 0
    elif moduleGlobal < modulesInPanels[3]:
        return 1
    else:
        return None


def isLargePD(moduleGlobal):
    if rich(moduleGlobal) == 0:
        return False
    else:
        if moduleGlobal % 6 in [2, 3]:
            return False
        elif moduleGlobal % 6 in [0, 1, 4, 5]:
            return True
        else:
            return None


def smartId(moduleGlobal, pdInModule):
    id = gbl.LHCb.RichSmartID()
    id.setIDType(gbl.LHCb.RichSmartID.MaPMTID)  # ! set first

    id.setRich(rich(moduleGlobal))
    id.setPanel(panel(moduleGlobal))
    id.setLargePMT(isLargePD(moduleGlobal))

    id.setPD(moduleGlobal, pdInModule)

    return id


# main part
def main():

    print(
        'Generating all possible PD-level smartIDs in the RICH numbering scheme.\n'
    )

    print(f'Recreating the output area: {outputPath}\n')
    if os.path.exists(outputPath):
        shutil.rmtree(outputPath)
    os.makedirs(outputPath)

    with open(
            os.path.join(outputPath, outputFileName),
            'w',
            encoding='utf8',
            newline='') as outputFile:

        # R1
        for module in range(nModulesR1):
            for pd in range(nPDsInModule):
                outputFile.write(f'{smartId(module,pd).key()}\t')

        outputFile.write('\n\n')

        # R2
        for module in range(nModulesR1, nModulesR1 + nModulesR2):
            for pd in range(nPDsInModule):
                outputFile.write(f'{smartId(module,pd).key()}\t')


if __name__ == '__main__':
    main()
