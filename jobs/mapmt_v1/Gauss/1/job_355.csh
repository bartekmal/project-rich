#!/bin/csh
setenv TOP_DIR_SIM /eos/lhcb/user/b/bmalecki/RICH_Upgrade/gauss/mapmt_v1
setenv JOB_NAME    355
setenv FIRST_EVENT 17701
setenv NUM_EVENTS  50
setenv RUN_NUMBER  13571
setenv PROD_NAME   1
setenv EVENT_CODE  13104011

if ( ! -e $TOP_DIR_SIM ) then
  echo "TOP_DIR_SIM does not exists: $TOP_DIR_SIM"
  echo "Fatal error. Exit."
  exit
endif

# Work dir related to PROD_NAME 
setenv PROD_DIR $TOP_DIR_SIM/$PROD_NAME
if ( ! -e $PROD_DIR ) then
  echo "PROD_DIR does not exists: $PROD_DIR"
  echo "Fatal error. Exit."
  exit
endif

# dir related to PROD_NAME to store dst 
setenv PROD_DIR_DATA $TOP_DIR_SIM/$PROD_NAME/data
if ( ! -e $PROD_DIR_DATA ) then
  echo "PROD_DIR_DATA does not exists: $PROD_DIR_DATA"
  echo "Fatal error. Exit."
  exit
endif

# dir related to PROD_NAME to store logs etc 
setenv PROD_DIR_LOGS $TOP_DIR_SIM/$PROD_NAME/logs
if ( ! -e $PROD_DIR_LOGS ) then
  echo "PROD_DIR_LOGS does not exists: $PROD_DIR_LOGS"
  echo "Fatal error. Exit."
  exit
endif


# Dir where job is executed
setenv JOB_DIR $PROD_DIR/work/$JOB_NAME
mkdir $JOB_DIR
cd $JOB_DIR

cp $RICH_BASE_OPTIONS/Gauss/Gauss-Job-Generic.py ./Gauss-Job.py
echo "LHCbApp().EvtMax = $NUM_EVENTS"           >> Gauss-Job.py
echo 'GaussGen = GenInit("GaussGen")' >> Gauss-Job.py
echo "GaussGen.FirstEventNumber = $FIRST_EVENT" >> Gauss-Job.py
echo "GaussGen.RunNumber        = $RUN_NUMBER"  >> Gauss-Job.py

$RICH_BASE_SOFTWARE/Gauss/build.x86_64-slc6-gcc7-opt/run -s GITCONDDBPATH=$RICH_BASE_SOFTWARE gaudirun.py Gauss-Job.py > gauss_${JOB_NAME}.log

setenv sim_file_name `ls *.sim`
mv $sim_file_name  $PROD_DIR_DATA/Gauss_${JOB_NAME}.sim
mv gauss_${JOB_NAME}.log $PROD_DIR_LOGS/
rm core.*
#mv GeneratorLog.xml $PROD_DIR_LOGS/GeneratorLog_${JOB_NAME}.xml
#cd ../
#rm -rf $JOB_DIR
