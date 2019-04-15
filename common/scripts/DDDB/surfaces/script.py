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

def createSurfacesForPmt(moduleGlobal, ecInModule, pmtInEc, ecInModuleBegin):

    pmtGlobal = moduleGlobal*16 + ecInModule*4 + pmtInEc
    f = open(outputPath+"/"+catalogPath+'/RichPMTSurface.xml:'+strExtend_4(pmtGlobal),'w')

    #header
    f.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    f.write('<!DOCTYPE DDDB  SYSTEM   "git:/DTD/geometry.dtd"[\n')

    #entities
    f.write('<!ENTITY ModuleNum  "' + strExtend_3(moduleGlobal) + '">\n')
    f.write('<!ENTITY EcNum   "' + str(ecInModule) + '">\n')
    f.write('<!ENTITY PmtNum  "' + str(pmtInEc) + '">\n')
    f.write('<!ENTITY SurfaceNum "' + strExtend_4(pmtGlobal) + '">\n')
    f.write('\n')
    f.write('<!ENTITY RichPMTSurfaceGeneric  SYSTEM   "git:/URich1/Surface/RichPMTSurfaceGeneric.xml">\n')
    
    #bottom
    f.write(']>'"\n\n")
    f.write('<DDDB> &RichPMTSurfaceGeneric;  </DDDB>'+"\n")

    f.close()

    surfaceListNames = [
        "RichPMTQuartzWindowSurface",
        "RichPMTQuartzWindowBackSurface",
        "RichPMTEnvSideTubeMetalSurface",
        "RichPMTBackEnvelopeMetalSurface",
        "RichPMTSiSensorSurface"
    ]
    for s in surfaceListNames:
        surfaceListFile.write('\t<surfaceref href="' + catalogPath + '/RichPMTSurface.xml:' + strExtend_4(pmtGlobal) + '#' + s + strExtend_4(pmtGlobal) + '"/>\n')

    surfaceListNamesPerModule = [
        "RichPMTQWPhCathodeSurface",
        "RichPMTQWPhCathodeBackSurface"
    ]
    #once per module...
    if ecInModule == ecInModuleBegin and pmtInEc == 0:
        for s in surfaceListNamesPerModule:
            surfaceListFile.write('\t<surfaceref href="' + catalogPath + '/RichPMTSurface.xml:' + strExtend_4(pmtGlobal) + '#' + s + strExtend_4(pmtGlobal) + '"/>\n')

def createSurfacesForGrandPmt(moduleGlobal, ecInModule):

    pmtGlobal = moduleGlobal*16 + ecInModule
    f = open(outputPath+"/"+catalogPath+'/RichGrandPMTSurface.xml:'+strExtend_4(pmtGlobal),'w')

    #header
    f.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    f.write('<!DOCTYPE DDDB  SYSTEM   "git:/DTD/geometry.dtd"[\n')

    #entities
    f.write('<!ENTITY ModuleNum  "' + strExtend_3(moduleGlobal) + '">\n')
    f.write('<!ENTITY EcNum   "' + str(ecInModule) + '">\n')
    f.write('<!ENTITY PmtNum  "0">\n')
    f.write('<!ENTITY SurfaceNum "' + strExtend_4(pmtGlobal) + '">\n')
    f.write('\n')
    f.write('<!ENTITY RichGrandPMTSurfaceGeneric  SYSTEM   "git:/URich1/Surface/RichGrandPMTSurfaceGeneric.xml">\n')
    
    #bottom
    f.write(']>'"\n\n")
    f.write('<DDDB> &RichGrandPMTSurfaceGeneric;  </DDDB>'+"\n")

    f.close()

    surfaceListNames = [
        "RichGrandPMTQuartzWindowSurface",
        "RichGrandPMTQuartzWindowBackSurface",
        "RichGrandPMTEnvSideTubeMetalSurface",
        "RichGrandPMTBackEnvelopeMetalSurface",
        "RichGrandPMTSiSensorSurface"
    ]
    for s in surfaceListNames:
        surfaceListFile.write('\t<surfaceref href="' + catalogPath + '/RichGrandPMTSurface.xml:' + strExtend_4(pmtGlobal) + '#' + s + strExtend_4(pmtGlobal) + '"/>\n')

    surfaceListNamesPerModule = [
        "RichGrandPMTQWPhCathodeSurface",
        "RichGrandPMTQWPhCathodeBackSurface"
    ]
    #once per module...
    if ecInModule == 0:
        for s in surfaceListNamesPerModule:
            surfaceListFile.write('\t<surfaceref href="' + catalogPath + '/RichGrandPMTSurface.xml:' + strExtend_4(pmtGlobal) + '#' + s + strExtend_4(pmtGlobal) + '"/>\n')

def createSurfacesForModule(moduleGlobal, ecInModuleBegin = 0, ecInModuleEnd = 4, pmtInEcBegin = 0, pmtInEcEnd = 4):
    for ecInModule in range(ecInModuleBegin, ecInModuleEnd):
            for pmtInEc in range(pmtInEcBegin, pmtInEcEnd):         
                createSurfacesForPmt(moduleGlobal, ecInModule, pmtInEc, ecInModuleBegin)

def createSurfacesForGrandModule(moduleGlobal):
    for ecInModule in range(0, 4):
            createSurfacesForGrandPmt(moduleGlobal, ecInModule)


#################################################################################

def surfaceListFileOpen(filePath):

    surfaceListFile = open(filePath,'w')

    surfaceListFile.write('<?xml version="1.0" encoding="ISO-8859-1"?>\n')
    surfaceListFile.write('<!DOCTYPE DDDB SYSTEM "git:/DTD/geometry.dtd">\n\n')
    surfaceListFile.write('<DDDB>\n\n')
    surfaceListFile.write(' '
                          '<catalog name="RichPMTSurfaceTabProperties" >\n'
                          '     <tabpropertyref href="RichPMTSurfaceTabProperty.xml#RichPMTQuartzWindowSurfaceReflectivityPT" />\n'
                          '     <tabpropertyref href="RichPMTSurfaceTabProperty.xml#RichPMTQuartzWindowSurfaceEfficiencyPT" />\n'
                          '     <tabpropertyref href="RichPMTSurfaceTabProperty.xml#RichPMTQWPhCathodeSurfaceReflectivityPT" />\n'
                          '     <tabpropertyref href="RichPMTSurfaceTabProperty.xml#RichPMTQWPhCathodeSurfaceEfficiencyPT" />\n'
                          '     <tabpropertyref href="RichPMTSurfaceTabProperty.xml#RichPMTMetalTubeSurfaceReflectivityPT" />\n'
                          '     <tabpropertyref href="RichPMTSurfaceTabProperty.xml#RichPMTMetalTubeSurfaceEfficiencyPT" />\n'
                          '     <tabpropertyref href="RichPMTSurfaceTabProperty.xml#RichPMTSiSensorSurfaceReflectivityPT" />\n'
                          '     <tabpropertyref href="RichPMTSurfaceTabProperty.xml#RichPMTSiSensorSurfaceEfficiencyPT" />\n'
                          '</catalog>\n\n'
                          '')
    surfaceListFile.write('<catalog name="RichPMTSurfaces" >\n')
    surfaceListFile.write('')

    return surfaceListFile

def surfaceListFileClose(surfaceListFile):

    surfaceListFile.write('</catalog>\n\n')
    surfaceListFile.write('</DDDB>\n')

    surfaceListFile.close()


######################################################################3

import os
outputPath = "output"
os.mkdir(outputPath)
catalogPath = "RichPMTSurfaceCatalog"
os.mkdir(outputPath + "/" + catalogPath)

surfaceListFile = surfaceListFileOpen(outputPath+'/RichPMTSurfaces.xml')

#RICH1
numberOfModulesRich1 = 132
noEC0modules = range(0,126,6)
noEC3modules = range(5,131,6)

print noEC0modules
print noEC3modules

for moduleGlobal in range(0,numberOfModulesRich1):

    #no EC01 modules
    if moduleGlobal == 60 or moduleGlobal == 126:
        createSurfacesForModule(moduleGlobal, 2, 4, 0, 4)
    #no EC23 modules
    elif moduleGlobal == 65 or moduleGlobal == 131:
        createSurfacesForModule(moduleGlobal, 0, 2, 0, 4)
    #no EC0 modules    
    elif moduleGlobal in noEC0modules:
        createSurfacesForModule(moduleGlobal, 1, 4, 0, 4)
    #no EC3 modules    
    elif moduleGlobal in noEC3modules:
        createSurfacesForModule(moduleGlobal, 0, 3, 0, 4)    
    #StdModules
    else:    
        createSurfacesForModule(moduleGlobal, 0, 4, 0, 4)

#RICH2
numberOfModulesRich2 = 144

for moduleGlobal in range(numberOfModulesRich1,numberOfModulesRich1+numberOfModulesRich2):

    if not isGrandModule(moduleGlobal):
        createSurfacesForModule(moduleGlobal, 0, 4, 0, 4)
    else:
        createSurfacesForGrandModule(moduleGlobal)

surfaceListFileClose(surfaceListFile)

