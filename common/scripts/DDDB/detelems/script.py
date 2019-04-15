#! /usr/bin/env python

def strExtend_3(nr):
    nrAsString=str(nr)
    if nr < 10:
        nrAsString = "00" + nrAsString
    elif nr < 100:
        nrAsString = "0" + nrAsString
    return nrAsString

def strExtend_4(nr):
    nrAsString=str(nr)
    if nr < 10:
        nrAsString = "000" + nrAsString
    elif nr < 100:
        nrAsString = "00" + nrAsString
    elif nr < 1000:
        nrAsString = "0" + nrAsString
    return nrAsString

def isGrandModule(moduleGlobal):
    if moduleGlobal < numberOfModulesRich1:
        return False
    else:
        if moduleGlobal % 6 in [2, 3]:
            return False
        elif moduleGlobal % 6 in [0, 1, 4, 5]:
            return True
        else:
            print "I have a bad feeling about this..."

def getPanelIndex(moduleGlobal):
    
    panelID = -1

    if moduleGlobal < numberOfModulesRich1:
        if moduleGlobal < numberOfModulesRich1/2:
            panelID = 0
        else:
            panelID = 1
    else:
        if moduleGlobal < numberOfModulesRich1 + numberOfModulesRich2/2:
            panelID = 0
        else:
            panelID = 1

    return panelID

#################################################################################

def myFileOpen(filePath):

    myFile = open(filePath,'w')

    myFile.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    myFile.write('<author>Sajan Easo</author>\n\n')
    
    myFile.write('')

    return myFile

def myFileClose(myFile):

    myFile.close()

######################################################################3

def createForModule(f, config, moduleGlobal, ecInModuleBegin = 0, ecInModuleEnd = 4, pmtInEcBegin = 0, pmtInEcEnd = 4):

    #module
    f.write('<detelem classID="2" name="MAPMT_MODULE:' + str(moduleGlobal) + '">\n')
    f.write('  <version>0.2</version>\n')
    f.write( moduleGeometryInfo[config].format(strExtend_3(moduleGlobal)) )
    f.write( moduleSupport[config].format(str(getPanelIndex(moduleGlobal))) )
    f.write( moduleNPath[config].format(strExtend_3(moduleGlobal), str(moduleGlobal)) )

    for ecInModule in range(ecInModuleBegin, ecInModuleEnd):
            for pmtInEc in range(pmtInEcBegin, pmtInEcEnd):         
                createForPmt(f, config, moduleGlobal, ecInModule, pmtInEc)
    
    f.write('</detelem>\n\n')

def createForPmt(f, config, moduleGlobal, ecInModule, pmtInEc):
    
    ecGlobal = moduleGlobal*4 + ecInModule
    pmtGlobal = -1
    if isGrandModule(moduleGlobal):
        pmtGlobal = moduleGlobal*16 + ecInModule
    else:
        pmtGlobal = moduleGlobal*16 + ecInModule*4 + pmtInEc
    
    #pmt
    f.write('  <detelem classID="12025" name="MAPMT:{0}">\n'.format(str(pmtGlobal)) )
    f.write( pmtGeometryInfo[config].format( str(pmtInEc), str(ecInModule), strExtend_3(moduleGlobal) ))
    f.write( pmtSupport[config].format(str(getPanelIndex(moduleGlobal)), str(moduleGlobal)) )
    f.write( pmtNPath[config].format(str(ecInModule), strExtend_3(moduleGlobal), str(ecGlobal), str(pmtInEc), str(pmtGlobal)) )
    #anode
    f.write('    <detelem classID="2"  name="MAPMTAnode:{0}">\n'.format(str(pmtGlobal)))
    f.write( anodeGeometryInfo[config] )
    f.write( anodeSupport[config].format(str(getPanelIndex(moduleGlobal)),str(moduleGlobal), str(pmtGlobal)) )
    f.write( anodeNPath[config].format(str(pmtInEc), str(ecInModule), strExtend_3(moduleGlobal)) )
    f.write('    </detelem>\n')
    f.write('  </detelem>\n\n')

###############################################################################3

def createModuleRich1(f, moduleGlobal):
    
    if moduleGlobal in noEC01modules:
        createForModule(f, 3, moduleGlobal, 2, 4, 0, 4)
    elif moduleGlobal in noEC23modules:
        createForModule(f, 4, moduleGlobal, 0, 2, 0, 4)
    elif moduleGlobal in noEC0modules:
        createForModule(f, 1, moduleGlobal, 1, 4, 0, 4)
    elif moduleGlobal in noEC3modules:
        createForModule(f, 2, moduleGlobal, 0, 3, 0, 4)  
    else:
        createForModule(f, 0, moduleGlobal, 0, 4, 0, 4)  

def createModuleRich2(f, moduleGlobal):

    if not isGrandModule(moduleGlobal):
        createForModule(f, 5, moduleGlobal, 0, 4, 0, 4)  
    else:
        createForModule(f, 6, moduleGlobal, 0, 4, 0, 1)  

######################################################################

moduleGeometryInfo = [
    '  <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1PmtModuleLogList/lvRich1PmtStdModule{0}"\n',
    '  <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1PmtModuleLogList/lvRich1PmtNoEC0TypeModule{0}"\n',
    '  <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1PmtModuleLogList/lvRich1PmtNoEC3TypeModule{0}"\n',
    '  <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1PmtModuleLogList/lvRich1PmtNoEC01TypeModule{0}"\n',
    '  <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1PmtModuleLogList/lvRich1PmtNoEC23TypeModule{0}"\n',
    '  <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/Rich2StdPmtModuleLogList/lvRich2PmtStdModule{0}"\n',
    '  <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/Rich2GrandPmtModuleLogList/lvRich2PmtGrandModule{0}"\n'
]

moduleSupport = [
    '  support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}"\n',
    '  support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}"\n',
    '  support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}"\n',
    '  support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}"\n',
    '  support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}"\n',
    '  support="/dd/Structure/LHCb/AfterMagnetRegion/Rich2/PMTPanel{0}"\n',
    '  support="/dd/Structure/LHCb/AfterMagnetRegion/Rich2/PMTPanel{0}"\n'
]

moduleNPath = [
    '  npath="pvRich1PmtStdModule{0}:{1}" />\n\n',
    '  npath="pvRich1PmtNoEC0TypeModule{0}:{1}" />\n\n',
    '  npath="pvRich1PmtNoEC3TypeModule{0}:{1}" />\n\n',
    '  npath="pvRich1PmtNoEC01TypeModule{0}:{1}" />\n\n',
    '  npath="pvRich1PmtNoEC23TypeModule{0}:{1}" />\n\n',
    '  npath="pvRich2PmtStdModule{0}:{1}" />\n\n',
    '  npath="pvRich2PmtGrandModule{0}:{1}" />\n\n'
]

pmtGeometryInfo = [
    '    <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/RichPmtLogList/lvRichPMTMasterP{0}InEC{1}InModule{2}"\n',
    '    <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/RichPmtLogList/lvRichPMTMasterP{0}InEC{1}InModule{2}"\n',
    '    <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/RichPmtLogList/lvRichPMTMasterP{0}InEC{1}InModule{2}"\n',
    '    <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/RichPmtLogList/lvRichPMTMasterP{0}InEC{1}InModule{2}"\n',
    '    <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/RichPmtLogList/lvRichPMTMasterP{0}InEC{1}InModule{2}"\n',
    '    <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/RichPmtLogList/lvRichPMTMasterP{0}InEC{1}InModule{2}"\n',
    '    <geometryinfo lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/Rich2GrandPmtLogList/lvRichGrandPMTMasterP{0}InECH{1}InGrandModule{2}"\n'
]

pmtSupport = [
    '    support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}"\n',
    '    support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}"\n',
    '    support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}"\n',
    '    support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}"\n',
    '    support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}"\n',
    '    support="/dd/Structure/LHCb/AfterMagnetRegion/Rich2/PMTPanel{0}/MAPMT_MODULE:{1}"\n',
    '    support="/dd/Structure/LHCb/AfterMagnetRegion/Rich2/PMTPanel{0}/MAPMT_MODULE:{1}"\n'
]

pmtNPath = [
    '    npath="pvRichPmtStdECR{0}InModule{1}:{2}/pvRichPMTMasterP{3}InEC{0}InModule{1}:{4}" />\n',
    '    npath="pvRichPmtStdECR{0}InNoEC0TypeModule{1}:{2}/pvRichPMTMasterP{3}InEC{0}InModule{1}:{4}" />\n',
    '    npath="pvRichPmtStdECR{0}InNoEC3TypeModule{1}:{2}/pvRichPMTMasterP{3}InEC{0}InModule{1}:{4}" />\n',
    '    npath="pvRichPmtStdECR{0}InNoEC01TypeModule{1}:{2}/pvRichPMTMasterP{3}InEC{0}InModule{1}:{4}" />\n',
    '    npath="pvRichPmtStdECR{0}InNoEC23TypeModule{1}:{2}/pvRichPMTMasterP{3}InEC{0}InModule{1}:{4}" />\n',
    '    npath="pvRichPmtStdEC{0}InModule{1}:{2}/pvRichPMTMasterP{3}InEC{0}InModule{1}:{4}" />\n',
    '    npath="pvRichPmtGrandECH{0}InModule{1}:{2}/pvRichGrandPMTMasterP{3}InECH{0}InGrandModule{1}:{4}" />\n'
]

anodeGeometryInfo = [
    '      <geometryinfo  lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTAnode0000"\n',
    '      <geometryinfo  lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTAnode0000"\n',
    '      <geometryinfo  lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTAnode0000"\n',
    '      <geometryinfo  lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTAnode0000"\n',
    '      <geometryinfo  lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTAnode0000"\n',
    '      <geometryinfo  lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTAnode0000"\n',
    '      <geometryinfo  lvname="/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTAnode0001"\n'
]

anodeSupport = [
    '      support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}/MAPMT:{2}"\n',
    '      support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}/MAPMT:{2}"\n',
    '      support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}/MAPMT:{2}"\n',
    '      support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}/MAPMT:{2}"\n',
    '      support="/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PMTPanel{0}/MAPMT_MODULE:{1}/MAPMT:{2}"\n',
    '      support="/dd/Structure/LHCb/AfterMagnetRegion/Rich2/PMTPanel{0}/MAPMT_MODULE:{1}/MAPMT:{2}"\n',
    '      support="/dd/Structure/LHCb/AfterMagnetRegion/Rich2/PMTPanel{0}/MAPMT_MODULE:{1}/MAPMT:{2}"\n'
]

anodeNPath = [
    '      npath="pvRichPMTSMasterP{0}InEC{1}InModule{2}/pvRichPMTAnode0000" />\n',
    '      npath="pvRichPMTSMasterP{0}InEC{1}InModule{2}/pvRichPMTAnode0000" />\n',
    '      npath="pvRichPMTSMasterP{0}InEC{1}InModule{2}/pvRichPMTAnode0000" />\n',
    '      npath="pvRichPMTSMasterP{0}InEC{1}InModule{2}/pvRichPMTAnode0000" />\n',
    '      npath="pvRichPMTSMasterP{0}InEC{1}InModule{2}/pvRichPMTAnode0000" />\n',
    '      npath="pvRichPMTSMasterP{0}InEC{1}InModule{2}/pvRichPMTAnode0000" />\n',
    '      npath="pvRichGrandPMTSMasterP{0}InECH{1}InGrandModule{2}/pvRichPMTAnode0001" />\n'    
]    
   
###############################################################################################
 
import os
outputPath = "output"
os.mkdir(outputPath)

os.mkdir(outputPath+"/URich1")
os.mkdir(outputPath+"/URich1/DetElem")
os.mkdir(outputPath+"/URich2")
os.mkdir(outputPath+"/URich2/DetElem")

fileName = [
    "Rich1PMTsInPanel0.xml",
    "Rich1PMTsInPanel1.xml",
    "Rich2MixedPMTsInPanel0.xml",
    "Rich2MixedPMTsInPanel1.xml"
]

myFile = [
    myFileOpen(outputPath+"/URich1/DetElem/"+fileName[0]),
    myFileOpen(outputPath+"/URich1/DetElem/"+fileName[1]),
    myFileOpen(outputPath+"/URich2/DetElem/"+fileName[2]),
    myFileOpen(outputPath+"/URich2/DetElem/"+fileName[3]),
]

#RICH1
numberOfModulesRich1 = 132
noEC0modules = range(0,60,6)+range(66,126,6)
noEC3modules = range(5,65,6)+range(71,131,6)
noEC01modules = [60, 126]
noEC23modules = [65, 131]

for moduleGlobal in range(0,numberOfModulesRich1/2):
    createModuleRich1(myFile[0], moduleGlobal)
for moduleGlobal in range(numberOfModulesRich1/2,numberOfModulesRich1):
    createModuleRich1(myFile[1], moduleGlobal)

#RICH2 
numberOfModulesRich2 = 144

for moduleGlobal in range(numberOfModulesRich1,numberOfModulesRich1+numberOfModulesRich2/2):
    createModuleRich2(myFile[2], moduleGlobal)
for moduleGlobal in range(numberOfModulesRich1+numberOfModulesRich2/2,numberOfModulesRich1+numberOfModulesRich2):
    createModuleRich2(myFile[3], moduleGlobal)

#close
for f in myFile:
    myFileClose(f)

