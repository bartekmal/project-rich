from Configurables import LHCbApp, CondDB, UpdateManagerSvc,DDDBConf, PanoramixSys
from Gaudi.Configuration import *

lhcbApp  = LHCbApp()
conddb=CondDB()
conddb.Upgrade = True
conddb.LoadCALIBDB="HLT1"

lhcbApp.Simulation = True
lhcbApp.DDDBtag = "upgrade/RichUpgrade-MaPMTArrayUpdates"
lhcbApp.CondDBtag = "sim-20170301-vc-md100"

PanoramixSys().Upgrade=True







