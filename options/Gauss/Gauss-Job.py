eventType="13104011"
from Gaudi.Configuration import *
from Gauss.Configuration import *
from  Configurables import LHCbApp, DDDBConf

LHCbApp().DDDBtag   = "upgrade/RichUpgrade-MaPMTArrayUpdates"
LHCbApp().CondDBtag = "upgrade/RichUpgrade-MaPMTArrayUpdates"

importOptions("$APPCONFIGOPTS/Gauss/Beam7000GeV-md100-nu7.6.py")


from Configurables import CondDB
conddb = CondDB()
conddb.Upgrade = True
LHCbApp().Simulation = True
conddb.LoadCALIBDB = "HLT1"


Gauss.DetectorGeo = {"Detectors": [ 'VP',   'UT',    'FT', 'Rich1Pmt', 'Rich2Pmt',  'Ecal', 'Hcal', 'Muon', 'Magnet'] }
Gauss.DetectorSim = {"Detectors": [ 'VP',   'UT',   'FT',  'Rich1Pmt', 'Rich2Pmt',  'Ecal', 'Hcal', 'Muon', 'Magnet'] }
Gauss.DetectorMoni ={"Detectors": [ 'VP',   'UT',   'FT',  'Rich1Pmt', 'Rich2Pmt',  'Ecal', 'Hcal', 'Muon', 'Magnet'] }


LHCbApp().Simulation = True
Gauss().DataType="Upgrade"

GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber =1
GaussGen.RunNumber        =161459
LHCbApp().EvtMax=1


importOptions("$DECFILESROOT/options/"+eventType+".py")
importOptions("$LBPYTHIA8ROOT/options/Pythia8.py") #pythia 8 used
importOptions("$APPCONFIGOPTS/Gauss/G4PL_FTFP_BERT_EmNoCuts.py") 
importOptions("$APPCONFIGOPTS/Persistency/Compression-ZLIB-1.py")
importOptions("$APPCONFIGOPTS/Gauss/Upgrade.py")

Gauss().OutputType = 'SIM'

def AvoidMisAlign():
    from Configurables import GiGaGeo
    gigaGeo=GiGaGeo()
    gigaGeo.UseAlignment =False
    gigaGeo.AlignAllDetectors = False
def CkvConfig():
    from Configurables import GiGa ,GiGaPhysConstructorOpCkv
    GiGa=GiGa()
    GiGa.ModularPL.addTool( GiGaPhysConstructorOpCkv,name="GiGaPhysConstructorOpCkv" )
    GiGa.ModularPL.GiGaPhysConstructorOpCkv.ActivatePmtModuleSuppressSet5=True
    GiGa.ModularPL.GiGaPhysConstructorOpCkv.ActivatePmtSuppressSet0 = True

appendPostConfigAction(AvoidMisAlign)
#appendPostConfigAction(ActivateAnalysis)
appendPostConfigAction(CkvConfig)




