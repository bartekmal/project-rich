#!/usr/bin/env python3

# Built-in/Generic Imports
import math

from dataclasses import dataclass

from parse import parse

# Own modules
# from {path} import {class}

__author__ = 'Bartosz Malecki'
__email__ = 'bartosz.piotr.malecki@cern.ch'

# global variables

# TODO ? move somewhre else / outside of here
idPattern = '{}/{}/{}_COL{}/PDM{}/EC{}/PMT{}'

# basic numbers
nPanelsRich = 2
nColumnsPanelR1 = 11
nColumnsPanelR2 = 12
nModulesColumn = 6
nEcsModule = 4
nPmtsEc = 4
nChannelsPmt = 64

# actual numbers of installed / valid PDs
nPmtsR1 = 1888
nPmtsR2 = 1152

# derived numbers
nPmtsModule = nEcsModule * nPmtsEc
nPmtsColumn = nPmtsModule * nModulesColumn
nModulesR1 = nModulesColumn * nColumnsPanelR1 * nPanelsRich
pmtCopyNumberMaxR1 = nModulesR1 * nEcsModule * nPmtsEc
nModulesR2 = nModulesColumn * nColumnsPanelR2 * nPanelsRich
pmtCopyNumberMaxR2 = nModulesR2 * nEcsModule * nPmtsEc

moduleGlobalStart = {
    'R1U': 0,
    'R1D': math.floor(nModulesR1/2),
    'R2A': nModulesR1,
    'R2C': nModulesR1 + math.floor(nModulesR2/2)
}


@dataclass
class SmartId:
    '''Helper class for numbering scheme conversions.'''

    rich: str = '' # ! RICH1 / RICH2
    plane: str = '' # ! R1U / R1D / R2A / R2C
    col: int = 0
    module: int = 0
    ec: int = 0
    pmt: int = 0

    @classmethod
    def initFromIdString(cls, idString):
        # ! could be improved (reg expressions?)
        parsedId = parse(idPattern, idString)

        # ! account for the 14-column numbering schemere for R2 in the input files (keep the 12-column one locally for now)
        # TODO keep an eye on this (input file format & the locally assumed RICH2 column numbering scheme)
        return cls(parsedId[0], parsedId[1], (int(parsedId[3]) - 1 if parsedId[0] == 'RICH2' else int(parsedId[3])),
                   int(parsedId[4]), int(parsedId[5]), int(parsedId[6]))

    @classmethod
    def initFromCopyNumber(cls, copyNr):
        
        smartIdLocal = cls()

        copyNrLocal = copyNr

        # TODO ? improve (less hard-coding)
        # RICH & panel
        if copyNrLocal < pmtCopyNumberMaxR1:
            smartIdLocal.rich = 'RICH1'
            
            if copyNrLocal < pmtCopyNumberMaxR1 // 2:
                smartIdLocal.plane = 'R1U'
            else:
                smartIdLocal.plane = 'R1D'
                copyNrLocal -= pmtCopyNumberMaxR1 // 2
            
        elif copyNrLocal < pmtCopyNumberMaxR1 + pmtCopyNumberMaxR2:
            smartIdLocal.rich = 'RICH2'
            copyNrLocal -= pmtCopyNumberMaxR1

            if copyNrLocal < pmtCopyNumberMaxR2 // 2:
                smartIdLocal.plane = 'R2A'
            else:
                smartIdLocal.plane = 'R2C'
                copyNrLocal -= pmtCopyNumberMaxR2 // 2
            
        else:
            return None

        # (local) col & module & EC & PMT
        smartIdLocal.col, copyNrLocal = divmod(copyNrLocal, nPmtsColumn)
        smartIdLocal.module, copyNrLocal = divmod(copyNrLocal, nPmtsModule)

        # (local) EC & PMT
        # ! keep in mind numbering scheme for R/H-types
        if smartIdLocal.isLargePmt():
            smartIdLocal.ec = copyNrLocal
            smartIdLocal.pmt = 0
        else:
            smartIdLocal.ec, smartIdLocal.pmt = divmod(copyNrLocal, nPmtsEc)
              
        return smartIdLocal

    @classmethod
    def testInitFromCopyNumber(cls):
        print(cls.initFromCopyNumber(0))
        print(cls.initFromCopyNumber(2111))
        print(cls.initFromCopyNumber(2112))
        print(cls.initFromCopyNumber(2112+2304))
        print(cls.initFromCopyNumber(2112 // 2))
        print(cls.initFromCopyNumber((2112 // 2) - 1))

    def isRich1(self) -> bool:
        return (self.rich == "RICH1")

    def isRich2(self) -> bool:
        return (self.rich == "RICH2")

    def isPanel0(self) -> bool:
        return (self.plane == "R1U" or self.plane == "R2A")

    def isPanel1(self) -> bool:
        return (self.plane == "R1D" or self.plane == "R2C")

    def moduleGlobal(self) -> int:
        return moduleGlobalStart[self.plane] + self.col * nModulesColumn + self.module

    def moduleGlobalInDd4hepScheme(self) -> int:
        # ! 12->14 columns in R2 numbering scheme between DetDesc/DD4HEP
        # TODO keep an eye on this

        if self.isRich2():
            if self.isPanel0():
                return (self.moduleGlobal() + nModulesColumn)
            elif self.isPanel1():
                return (self.moduleGlobal() + nModulesColumn * 3)
            else:
                return None

        elif self.isRich1():
            return self.moduleGlobal()

        else:
            return None
        
    def pmtInModule(self) -> int:
        return (self.ec) if (self.isLargePmt()) else (self.ec * nPmtsEc + self.pmt)

    def copyNumber(self) -> int:
        '''Return PMT copy number (unique in the whole RICH system).'''
        return self.moduleGlobal() * nPmtsModule + self.pmtInModule()

    def copyNumberInDd4hepScheme(self) -> int:
        '''Return PMT copy number (unique in the whole RICH system) in DD4HEP scheme.'''
        # ! 12->14 columns in R2 numbering scheme between DetDesc/DD4HEP
        # TODO keep an eye on this

        if self.isRich2():
            if self.isPanel0():
                return (self.copyNumber() + nPmtsColumn)
            elif self.isPanel1():
                return (self.copyNumber() + nPmtsColumn * 3)
            else:
                return None

        elif self.isRich1():
            return self.copyNumber()

        else:
            return None

    def isLargePmt(self) -> bool:
        return self.isLargePmtFromGlobalModule(self.moduleGlobal())

    @staticmethod
    def isLargePmtFromGlobalModule(moduleGlobal):
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

        emptyModules = [0, 5, 66, 71]
        noEC0TypeModules = list(range(6, 66, 6))+list(range(72, 132, 6))
        noEC3TypeModules = list(range(11, 71, 6))+list(range(77, 137, 6))

        for pmtCopyNumber in range(copyNumberBegin, copyNumberEnd):
            moduleGlobal = int(pmtCopyNumber//16)
            pmtInModule = pmtCopyNumber % 16

            # RICH1
            if pmtCopyNumber < pmtCopyNumberMaxR1:
                # empty modules
                if moduleGlobal in emptyModules:
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
                if SmartId.isLargePmtFromGlobalModule(moduleGlobal) and pmtInModule >= 4:
                    continue
                else:
                    copyNumberList.append(pmtCopyNumber)

        return copyNumberList

    def stringFormattedInDd4hepScheme(self) -> str:
        ecSuffix = "H" if self.isLargePmt() else "R"
        modulePrefix = "" if not self.isRich2() else ("Grand" if self.isLargePmt() else "Std")

        return f"MAPMTP{self.pmt:1d}InEC{ecSuffix}{self.ec:1d}In{modulePrefix}Module{self.moduleGlobalInDd4hepScheme():03d}"
