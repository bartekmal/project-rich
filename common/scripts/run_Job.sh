#!/bin/sh

#load RICH environment
source $RICH_BASE/setupRICH.sh
echo ""

#renew Kerberos tickets
#echo "Renew Kerberos tickets: "
#kinit
#echo "<-- Done!"
#echo ""

######################################################## BEGIN CONFIG ###################

# Script will create NUM_JOBS with EVT_PER_JOB. Adjust JOB_FLAVOUR and MAX_RUNTIME to the given EVT_PER_JOB.
# Generation seed is taken from RUN_NUMBER and event number --> !!!!!! provide a unique RUN_NUMBER !!!!!!
# DB tags can be set by DDDB / CONDDB.
# Option file and the version of application can be set by: GENERIC_OPTIONS and RUN_COMMAND
# The script should be executed in the jobs/ directory - the proper OUTPUT_DIR is created on EOS. If OUTPUT_DIR already exists, the script asks if it should be cleaned up (! removes whole folder OUTPUT_DIR)

##timing on HTCondor:
## 5k EV: no visible stat errors in PID plot (2k ok for checks)
## PID  : 2k events - 4h
## Gauss: 25 B-events - 15h
## Gauss: 10k pGun events - 6h

#job flavours in condor
#espresso     = 20 mins
#microcentury = 1 hour
#longlunch    = 2 hours
#workday      = 8 hours
#tomorrow     = 1 day
#testmatch    = 3 days
#nextweek     = 1 week

# number of events / jobs + batch settings

EVT_PER_JOB=25
NUM_JOBS=400
RUN_NUMBER=15375

JOB_FLAVOUR="tomorrow" #queue type in condor
MAX_RUNTIME=86400 #in secs

#JOB_FLAVOUR="microcentury" #queue type in condor
#MAX_RUNTIME=3600 #in secs

# Gauss / DDDB / options to run
DDDB=upgrade/RichUpgrade-MaPMTArrayUpdates
CONDDB=upgrade/RichUpgrade-MaPMTArrayUpdates
#DDDB=upgrade/dddb-20190223
#CONDDB=upgrade/sim-20180530-vc-md100

#replace with local files if needed
GENERIC_OPTIONS_GAUSS=${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-Generic.py
#GENERIC_OPTIONS_GAUSS=${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Rich1-Generic.py
#GENERIC_OPTIONS_GAUSS=${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Rich2-Generic.py
GENERIC_OPTIONS_BOOLE=${RICH_BASE_OPTIONS}/Boole/Boole-Job-Generic.py
GENERIC_OPTIONS_BRUNEL=${RICH_BASE_OPTIONS}/Brunel/Brunel-Job-Generic.py

RUN_COMMAND_GAUSS="$RICH_BASE_SOFTWARE/stack_Gauss/Gauss/build.${CMTCONFIG_GAUSS}/run -s GITCONDDBPATH=$RICH_BASE_SOFTWARE"
RUN_COMMAND_BOOLE="$RICH_BASE_SOFTWARE/Boole/build.${CMTCONFIG}/run -s GITCONDDBPATH=$RICH_BASE_SOFTWARE"
RUN_COMMAND_BRUNEL="$RICH_BASE_SOFTWARE/Brunel/build.${CMTCONFIG}/run -s GITCONDDBPATH=$RICH_BASE_SOFTWARE"

#RUN_COMMAND_GAUSS="$RICH_BASE_SOFTWARE/Gauss_v53r1/build.${CMTCONFIG_GAUSS}/run -s GITCONDDBPATH=$RICH_BASE_SOFTWARE"
#RUN_COMMAND_BOOLE="$RICH_BASE_SOFTWARE/Boole_v40r3/build.${CMTCONFIG}/run -s GITCONDDBPATH=$RICH_BASE_SOFTWARE"
#RUN_COMMAND_BRUNEL="$RICH_BASE_SOFTWARE/Brunel_v60r3/build.${CMTCONFIG}/run -s GITCONDDBPATH=$RICH_BASE_SOFTWARE"

LIST_TO_RUN="Gauss Boole Brunel" #Gauss Boole Brunel

############################################################ END OF CONFIG #######################

#set flags for applications to run

RUN_GAUSS=0
RUN_BOOLE=0
RUN_BRUNEL=0

for ITEM in ${LIST_TO_RUN}; do
    if [[ $ITEM == "Gauss" ]]; then RUN_GAUSS=1; fi
    if [[ $ITEM == "Boole" ]]; then RUN_BOOLE=1; fi
    if [[ $ITEM == "Brunel" ]]; then RUN_BRUNEL=1; fi
done

############## check if in jobs/
if [[ $PWD != *"jobs/"* ]]; then
    echo "You are not in jobs/ subdirectory!"
    while true; do
    read -p "Do you want to continue? " yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) echo "Exiting..."; exit;;
        * ) echo "Please answer y/n.";;
    esac
done
fi

JOB_NAME=`echo $PWD | sed -e 's/^.*jobs//'`
SUBMIT_DIR=$PWD

mkdir tmp; cd tmp

##############create dirs on eos

OUTPUT_DIR=${RICH_DATA}${JOB_NAME} #no / on purpose, works!

if [ -e $OUTPUT_DIR ]; then
    echo "Directory already exists:"
    echo "$OUTPUT_DIR"
    while true; do
      read -p "Do you want to clean-up dir and continue? ($LIST_TO_RUN will be removed)" yn
      case $yn in
        [Yy]* ) for OUTPUT_DIR_SUB in ${LIST_TO_RUN}; do rm -rf $OUTPUT_DIR/${OUTPUT_DIR_SUB}; done; echo "Dir cleaned-up ."; echo ""; break;;
        [Nn]* ) echo "Exiting..."; exit;;
        * ) echo "Please answer y/n.";;
      esac
    done
else
    mkdir $OUTPUT_DIR;
fi

JOB_FOLDERS="data root log"

for OUTPUT_DIR_SUB in ${LIST_TO_RUN}
do
    if [ ! -e $OUTPUT_DIR/$OUTPUT_DIR_SUB ]; then
	mkdir -p $OUTPUT_DIR/$OUTPUT_DIR_SUB
	for FOLDER in ${JOB_FOLDERS}
	do	
	    mkdir $OUTPUT_DIR/$OUTPUT_DIR_SUB/$FOLDER
	done
    fi
done

######## create input (options) files

OPTIONS_DIR=${SUBMIT_DIR}/tmp/options

if [ -e $OPTIONS_DIR ]; then
    for OUTPUT_DIR_SUB in ${LIST_TO_RUN}
    do
	rm -rf ${OPTIONS_DIR}/${OUTPUT_DIR_SUB}; mkdir -p ${OPTIONS_DIR}/${OUTPUT_DIR_SUB}	
    done
else
    mkdir $OPTIONS_DIR
    for OUTPUT_DIR_SUB in ${LIST_TO_RUN}
    do
	mkdir -p ${OPTIONS_DIR}/${OUTPUT_DIR_SUB}	
    done
fi

#Gauss

if [[ $RUN_GAUSS == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Gauss
    OPTIONS_FILE=Gauss-Job

    for i in $(seq 0 $(( ${NUM_JOBS} - 1 )) )
    do
	touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "#! /usr/bin/env python" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "from  Configurables import LHCbApp, DDDBConf" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "LHCbApp().DDDBtag   = \"${DDDB}\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "LHCbApp().CondDBtag   = \"${CONDDB}\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

	cat ${GENERIC_OPTIONS_GAUSS} >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

	FIRST_EVENT=$(( ${i} * ${EVT_PER_JOB} ))
	echo "LHCbApp().EvtMax  = ${EVT_PER_JOB}" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "GaussGen = GenInit(\"GaussGen\")" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "GaussGen.FirstEventNumber   = ${FIRST_EVENT}" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "GaussGen.RunNumber   = ${RUN_NUMBER}" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
    done

fi

#Boole

if [[ $RUN_BOOLE == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Boole
    OPTIONS_FILE=Boole-Job

    for i in $(seq 0 $(( ${NUM_JOBS} - 1 )) )
    do
	touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

	echo "#! /usr/bin/env python" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "InputArea = \"${EOS_PREFIX}${OUTPUT_DIR}/Gauss/data\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	#echo "OutputArea = \".\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	
	echo "from  Configurables import LHCbApp, DDDBConf" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "LHCbApp().DDDBtag   = \"${DDDB}\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "LHCbApp().CondDBtag   = \"${CONDDB}\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	
	cat ${GENERIC_OPTIONS_BOOLE} >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

	echo "myInputFile = [ InputArea+\"/Gauss_${i}\" ]" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

	echo "EventSelector().Input =[ \"DATAFILE='PFN:%s.sim'  TYP='POOL_ROOTTREE'  OPT='READ'\"%tmp for tmp in myInputFile ]" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "Boole().EvtMax = ${EVT_PER_JOB}" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

    done

fi

#Brunel

if [[ $RUN_BRUNEL == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Brunel
    OPTIONS_FILE=Brunel-Job

    for i in $(seq 0 $(( ${NUM_JOBS} - 1 )) )
    do
	touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

	echo "#! /usr/bin/env python" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "InputArea = \"${EOS_PREFIX}${OUTPUT_DIR}/Boole/data\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	#echo "OutputArea = \".\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	
	echo "from  Configurables import LHCbApp, DDDBConf" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "LHCbApp().DDDBtag   = \"${DDDB}\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "LHCbApp().CondDBtag   = \"${CONDDB}\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

	cat ${GENERIC_OPTIONS_BRUNEL} >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

	echo "myInputFile = [ InputArea+\"/Boole_${i}\" ]" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

	echo "EventSelector().Input =[ \"DATAFILE='PFN:%s.digi'  TYP='POOL_ROOTTREE'  OPT='READ'\"%tmp for tmp in myInputFile ]" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
	echo "Brunel().EvtMax = ${EVT_PER_JOB}" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
    done

fi

##############prepare condor submission file
rm condor.sub
touch condor.sub

rm -rf output; mkdir output
rm -rf error; mkdir error
rm *.cc
rm condor.log

echo "executable = start_job.sh" >> condor.sub
echo "arguments = "'$(ProcId)' >> condor.sub
echo "" >> condor.sub
echo "log = condor.log" >> condor.sub
echo "output = output/condor_"'$(ProcId)'".out" >> condor.sub
echo "error = error/condor_"'$(ProcId)'".err" >> condor.sub
echo "" >> condor.sub
echo "+JobFlavour = ${JOB_FLAVOUR} " >> condor.sub
echo "+MaxRuntime = ${MAX_RUNTIME} " >> condor.sub
echo "" >> condor.sub
echo "queue ${NUM_JOBS}" >> condor.sub

######## prepare script start_job
rm start_job.sh
touch start_job.sh

echo "#!/bin/sh" >> start_job.sh
echo "" >> start_job.sh

#Gauss

if [[ $RUN_GAUSS == "1" ]]; then

    echo "cd "'$TMPDIR' >> start_job.sh
    echo "${RUN_COMMAND_GAUSS} gaudirun.py ${OPTIONS_DIR}/Gauss/Gauss-Job_"'${1}'".py > gauss.log" >> start_job.sh
    echo "" >> start_job.sh

    #get output
    echo "sleep $SLEEP_TIME" >> start_job.sh
    echo "eos cp *.sim $OUTPUT_DIR/Gauss/data/Gauss_"'${1}'".sim" >> start_job.sh
    echo "eos cp *.root $OUTPUT_DIR/Gauss/root/Gauss_"'${1}'".root" >> start_job.sh
    echo "eos cp *.log $OUTPUT_DIR/Gauss/log/Gauss_"'${1}'".log" >> start_job.sh

fi

#Boole

if [[ $RUN_BOOLE == "1" ]]; then

    echo "cd "'$TMPDIR' >> start_job.sh
    echo "${RUN_COMMAND_BOOLE} gaudirun.py ${OPTIONS_DIR}/Boole/Boole-Job_"'${1}'".py > boole.log" >> start_job.sh
    echo "" >> start_job.sh

    #get output
    echo "sleep $SLEEP_TIME" >> start_job.sh
    echo "eos cp *.digi $OUTPUT_DIR/Boole/data/Boole_"'${1}'".digi" >> start_job.sh
    echo "eos cp *.root $OUTPUT_DIR/Boole/root/Boole_"'${1}'".root" >> start_job.sh
    echo "eos cp *.log $OUTPUT_DIR/Boole/log/Boole_"'${1}'".log" >> start_job.sh

fi

#Brunel

if [[ $RUN_BRUNEL == "1" ]]; then

    echo "cd "'$TMPDIR' >> start_job.sh
    echo "${RUN_COMMAND_BRUNEL} gaudirun.py ${OPTIONS_DIR}/Brunel/Brunel-Job_"'${1}'".py > brunel.log" >> start_job.sh
    echo "" >> start_job.sh

    #get output
    echo "sleep $SLEEP_TIME" >> start_job.sh
    echo "eos cp Brunel-Ntuple.root $OUTPUT_DIR/Brunel/root/Brunel-Ntuple_"'${1}'".root" >> start_job.sh
    echo "eos cp Brunel-Histo.root $OUTPUT_DIR/Brunel/root/Brunel-Histo_"'${1}'".root" >> start_job.sh
    echo "eos cp brunel.log $OUTPUT_DIR/Brunel/log/Brunel_"'${1}'".log" >> start_job.sh

fi

###############submit job
echo "Created job ${JOB_NAME}:" >> ${SUBMIT_DIR}/job.log

echo "----> SUBMIT_DIR  : ${SUBMIT_DIR}" >> ${SUBMIT_DIR}/job.log
echo "----> OUTPUT_DIR  : ${OUTPUT_DIR}" >> ${SUBMIT_DIR}/job.log 
echo ""
echo "----> LIST_TO_RUN  : ${LIST_TO_RUN}" >> ${SUBMIT_DIR}/job.log
echo "----> RUN_NUMBER  : ${RUN_NUMBER}" >> ${SUBMIT_DIR}/job.log
echo "----> EVT_PER_JOB : ${EVT_PER_JOB}" >> ${SUBMIT_DIR}/job.log
echo "----> NUM_JOBS    : ${NUM_JOBS}" >> ${SUBMIT_DIR}/job.log
echo "----> JOB_FLAVOUR : ${JOB_FLAVOUR}" >> ${SUBMIT_DIR}/job.log
echo "----> MAX_RUNTIME : ${MAX_RUNTIME}" >> ${SUBMIT_DIR}/job.log
echo ""
echo "----> DDDB        : ${DDDB}" >> ${SUBMIT_DIR}/job.log
echo "----> CONDDB      : ${CONDDB}" >> ${SUBMIT_DIR}/job.log
echo ""
echo "----> RUN_COMMAND_GAUSS : ${RUN_COMMAND_GAUSS}" >> ${SUBMIT_DIR}/job.log
echo "----> RUN_COMMAND_BOOLE : ${RUN_COMMAND_BOOLE}" >> ${SUBMIT_DIR}/job.log
echo "----> RUN_COMMAND_BRUNEL : ${RUN_COMMAND_BRUNEL}" >> ${SUBMIT_DIR}/job.log
echo "" 
echo "----> GENERIC_OPTIONS_GAUSS : ${GENERIC_OPTIONS_GAUSS}" >> ${SUBMIT_DIR}/job.log
echo "----> GENERIC_OPTIONS_BOOLE : ${GENERIC_OPTIONS_BOOLE}" >> ${SUBMIT_DIR}/job.log
echo "----> GENERIC_OPTIONS_BRUNEL : ${GENERIC_OPTIONS_BRUNEL}" >> ${SUBMIT_DIR}/job.log
echo ""

echo "Submitting job ${JOB_NAME} to a queue"
chmod u+x start_job.sh

condor_submit condor.sub

