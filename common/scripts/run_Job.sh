#!/bin/sh

# check if RICH environment is set (needs a known host)
if [ -z ${RICH_HOST+x} ]; then
    echo -e "\nRICH environment not set. Exitting.\n"
    exit 1
fi

######################################################## BEGIN CONFIG ###################

# Script will create NUM_JOBS with EVT_PER_JOB. Adjust JOB_FLAVOUR and MAX_RUNTIME to the given EVT_PER_JOB.
# Generation seed is taken from RUN_NUMBER and event number --> !!!!!! provide a unique RUN_NUMBER !!!!!!
# DB tags can be set by DDDB / CONDDB.
# Option file and the version of application can be set by: GENERIC_OPTIONS and RUN_COMMAND
# The script should be executed in the jobs/ directory - the proper OUTPUT_DIR is created on EOS. If OUTPUT_DIR already exists, the script asks if it should be cleaned up (! removes whole folder OUTPUT_DIR)

# number of events / jobs + batch settings
EVT_PER_JOB=
NUM_JOBS=
RUN_NUMBER=

JOB_FLAVOUR= #queue type in condor
MAX_RUNTIME= #in secs

# Gauss / DDDB / options to run
DDDB=
CONDDB=

#replace with local files if needed
GENERIC_OPTIONS_GAUSS=
GENERIC_OPTIONS_BOOLE=
# GENERIC_OPTIONS_BOOLE=${PWD}/Boole-Job-Generic.py
GENERIC_OPTIONS_MOORE=
# GENERIC_OPTIONS_MOORE=${PWD}/Moore-Job-Generic.py
GENERIC_OPTIONS_BRUNEL=
GENERIC_OPTIONS_REC=

RUN_COMMAND_GAUSS=
RUN_COMMAND_BOOLE=
RUN_COMMAND_MOORE=
RUN_COMMAND_BRUNEL=
RUN_COMMAND_REC=

LIST_TO_RUN= #Gauss Boole Moore Brunel Rec Rec1 Rec2

############################################################ END OF CONFIG #######################

#set flags for applications to run

RUN_GAUSS=0
RUN_BOOLE=0
RUN_MOORE=0
RUN_BRUNEL=0
RUN_REC=0
RUN_REC_1=0
RUN_REC_2=0

for ITEM in ${LIST_TO_RUN}; do
    if [[ $ITEM == "Gauss" ]]; then RUN_GAUSS=1; fi
    if [[ $ITEM == "Boole" ]]; then RUN_BOOLE=1; fi
    if [[ $ITEM == "Moore" ]]; then RUN_MOORE=1; fi
    if [[ $ITEM == "Brunel" ]]; then RUN_BRUNEL=1; fi
    if [[ $ITEM == "Rec" ]]; then RUN_REC=1; fi
    if [[ $ITEM == "Rec1" ]]; then RUN_REC_1=1; fi
    if [[ $ITEM == "Rec2" ]]; then RUN_REC_2=1; fi
done

############## check if in jobs/
if [[ $PWD != *"jobs/"* ]]; then
    echo "You are not in jobs/ subdirectory!"
    while true; do
        read -p "Do you want to continue? " yn
        case $yn in
        [Yy]*) break ;;
        [Nn]*)
            echo "Exiting..."
            exit
            ;;
        *) echo "Please answer y/n." ;;
        esac
    done
fi

JOB_NAME=$(echo $PWD | sed -e 's/^.*jobs//')
SUBMIT_DIR=$PWD

mkdir tmp
cd tmp

##############create dirs on eos

OUTPUT_DIR=${RICH_DATA}${JOB_NAME} #no / on purpose, works!

if [ -e $OUTPUT_DIR ]; then
    echo "Directory already exists:"
    echo "$OUTPUT_DIR"
    while true; do
        read -p "Do you want to clean-up dir and continue? ($LIST_TO_RUN will be removed)" yn
        case $yn in
        [Yy]*)
            for OUTPUT_DIR_SUB in ${LIST_TO_RUN}; do eos rm -r ${OUTPUT_DIR}/${OUTPUT_DIR_SUB}; done
            echo "Dir cleaned-up ."
            echo ""
            break
            ;;
        [Nn]*)
            echo "Exiting..."
            exit
            ;;
        *) echo "Please answer y/n." ;;
        esac
    done
else
    eos mkdir ${OUTPUT_DIR}
fi

JOB_FOLDERS="data root log"

for OUTPUT_DIR_SUB in ${LIST_TO_RUN}; do
    if [ ! -d ${OUTPUT_DIR}/${OUTPUT_DIR_SUB} ]; then
        for FOLDER in ${JOB_FOLDERS}; do
            eos mkdir -p ${OUTPUT_DIR}/${OUTPUT_DIR_SUB}/${FOLDER}
        done
    fi
done

######## create input (options) files

OPTIONS_DIR=${SUBMIT_DIR}/tmp/options

if [ -e $OPTIONS_DIR ]; then
    for OUTPUT_DIR_SUB in ${LIST_TO_RUN}; do
        rm -rf ${OPTIONS_DIR}/${OUTPUT_DIR_SUB}
        mkdir -p ${OPTIONS_DIR}/${OUTPUT_DIR_SUB}
    done
else
    mkdir $OPTIONS_DIR
    for OUTPUT_DIR_SUB in ${LIST_TO_RUN}; do
        mkdir -p ${OPTIONS_DIR}/${OUTPUT_DIR_SUB}
    done
fi

#Gauss

if [[ $RUN_GAUSS == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Gauss
    OPTIONS_FILE=Gauss-Job

    for i in $(seq 0 $((${NUM_JOBS} - 1))); do
        touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "#! /usr/bin/env python" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "from  Configurables import LHCbApp, DDDBConf" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().DDDBtag   = \"${DDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().CondDBtag   = \"${CONDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        cat ${GENERIC_OPTIONS_GAUSS} >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        FIRST_EVENT=$((${i} * ${EVT_PER_JOB}))
        echo "LHCbApp().EvtMax  = ${EVT_PER_JOB}" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "GaussGen = GenInit(\"GaussGen\")" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "GaussGen.FirstEventNumber   = ${FIRST_EVENT}" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "GaussGen.RunNumber   = ${RUN_NUMBER}" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
    done

fi

#Boole

if [[ $RUN_BOOLE == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Boole
    OPTIONS_FILE=Boole-Job

    for i in $(seq 0 $((${NUM_JOBS} - 1))); do
        touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "#! /usr/bin/env python" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "InputArea = \"${EOS_PREFIX}${OUTPUT_DIR}/Gauss/data\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        #echo "OutputArea = \".\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "from  Configurables import LHCbApp, DDDBConf" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().DDDBtag   = \"${DDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().CondDBtag   = \"${CONDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        cat ${GENERIC_OPTIONS_BOOLE} >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "myInputFile = [ InputArea+\"/Gauss_${i}\" ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "EventSelector().Input =[ \"DATAFILE='PFN:%s.sim'  TYP='POOL_ROOTTREE'  OPT='READ'\"%tmp for tmp in myInputFile ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "Boole().EvtMax = ${EVT_PER_JOB}" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

    done

fi

#Moore

if [[ $RUN_MOORE == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Moore
    OPTIONS_FILE=Moore-Job

    for i in $(seq 0 $((${NUM_JOBS} - 1))); do
        touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "#! /usr/bin/env python" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "InputArea = \"${EOS_PREFIX}${OUTPUT_DIR}/Boole/data\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "from Moore import options" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "options.dddb_tag = \"${DDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "options.conddb_tag = \"${CONDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "myInputFile = [ InputArea+\"/Boole_${i}\" ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "options.input_files = [ \"PFN:%s.digi\"%tmp for tmp in myInputFile ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "options.evt_max = ${EVT_PER_JOB}" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py        
        cat ${GENERIC_OPTIONS_MOORE} >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
    done
fi

#Brunel

if [[ $RUN_BRUNEL == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Brunel
    OPTIONS_FILE=Brunel-Job

    for i in $(seq 0 $((${NUM_JOBS} - 1))); do
        touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "#! /usr/bin/env python" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "InputArea = \"${EOS_PREFIX}${OUTPUT_DIR}/Boole/data\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        #echo "OutputArea = \".\"" >> ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "from  Configurables import LHCbApp, DDDBConf" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().DDDBtag   = \"${DDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().CondDBtag   = \"${CONDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        cat ${GENERIC_OPTIONS_BRUNEL} >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "myInputFile = [ InputArea+\"/Boole_${i}\" ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "EventSelector().Input =[ \"DATAFILE='PFN:%s.digi'  TYP='POOL_ROOTTREE'  OPT='READ'\"%tmp for tmp in myInputFile ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "Brunel().EvtMax = ${EVT_PER_JOB}" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
    done

fi

#Rec

if [[ $RUN_REC == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Rec
    OPTIONS_FILE=Rec-Job

    for i in $(seq 0 $((${NUM_JOBS} - 1))); do
        touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "rads = [\"Rich1Gas\", \"Rich2Gas\"]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "nEvents = ${EVT_PER_JOB}" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        cat ${GENERIC_OPTIONS_REC} >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "InputArea = \"${EOS_PREFIX}${OUTPUT_DIR}/Brunel/data\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "myInputFile = [ InputArea+\"/Brunel_${i}\" ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "EventSelector().Input =[ \"DATAFILE='PFN:%s.xdst'  TYP='POOL_ROOTTREE'  OPT='READ'\"%tmp for tmp in myInputFile ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "HistogramPersistencySvc().OutputFile = \"Rec-Histo.root\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "GlobalRecoChecks().ProtoTupleName = \"Rec-Ntuple.root\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo " # DB tags settings" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "CondDB().Upgrade = True" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().DDDBtag   = \"${DDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().CondDBtag   = \"${CONDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().Simulation = True" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

    done

fi

if [[ $RUN_REC_1 == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Rec1
    OPTIONS_FILE=Rec-Job

    for i in $(seq 0 $((${NUM_JOBS} - 1))); do
        touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "rads = [\"Rich1Gas\"]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "nEvents = ${EVT_PER_JOB}" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        cat ${GENERIC_OPTIONS_REC} >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "InputArea = \"${EOS_PREFIX}${OUTPUT_DIR}/Brunel/data\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "myInputFile = [ InputArea+\"/Brunel_${i}\" ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "EventSelector().Input =[ \"DATAFILE='PFN:%s.xdst'  TYP='POOL_ROOTTREE'  OPT='READ'\"%tmp for tmp in myInputFile ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "HistogramPersistencySvc().OutputFile = \"Rec1-Histo.root\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "GlobalRecoChecks().ProtoTupleName = \"Rec1-Ntuple.root\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo " # DB tags settings" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "CondDB().Upgrade = True" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().DDDBtag   = \"${DDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().CondDBtag   = \"${CONDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().Simulation = True" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

    done

fi

if [[ $RUN_REC_2 == "1" ]]; then

    OPTIONS_DIR_TMP=${OPTIONS_DIR}/Rec2
    OPTIONS_FILE=Rec-Job

    for i in $(seq 0 $((${NUM_JOBS} - 1))); do
        touch ${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "rads = [\"Rich2Gas\"]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "nEvents = ${EVT_PER_JOB}" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        cat ${GENERIC_OPTIONS_REC} >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "InputArea = \"${EOS_PREFIX}${OUTPUT_DIR}/Brunel/data\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "myInputFile = [ InputArea+\"/Brunel_${i}\" ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "EventSelector().Input =[ \"DATAFILE='PFN:%s.xdst'  TYP='POOL_ROOTTREE'  OPT='READ'\"%tmp for tmp in myInputFile ]" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo "HistogramPersistencySvc().OutputFile = \"Rec2-Histo.root\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "GlobalRecoChecks().ProtoTupleName = \"Rec2-Ntuple.root\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

        echo " # DB tags settings" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "CondDB().Upgrade = True" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().DDDBtag   = \"${DDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().CondDBtag   = \"${CONDDB}\"" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "LHCbApp().Simulation = True" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py
        echo "" >>${OPTIONS_DIR_TMP}/${OPTIONS_FILE}_${i}.py

    done

fi

##############prepare condor submission file
rm -f condor.sub
touch condor.sub

rm -rf output
mkdir output
rm -rf error
mkdir error
rm -f *.cc
rm -f condor.log

echo "executable = start_job.sh" >>condor.sub
echo "arguments = "'$(ProcId)' >>condor.sub
echo "" >>condor.sub
echo "log = condor.log" >>condor.sub
echo "output = output/condor_"'$(ProcId)'".out" >>condor.sub
echo "error = error/condor_"'$(ProcId)'".err" >>condor.sub
echo "" >>condor.sub
echo "+JobFlavour = ${JOB_FLAVOUR} " >>condor.sub
echo "+MaxRuntime = ${MAX_RUNTIME} " >>condor.sub
echo "+AccountingGroup = \"${RICH_COMPUTING_GROUP}\"" >>condor.sub
echo "" >>condor.sub
echo "queue ${NUM_JOBS}" >>condor.sub

######## prepare script start_job
rm -f start_job.sh
touch start_job.sh

echo "#!/bin/sh" >>start_job.sh
echo "" >>start_job.sh

# pass environment variables to the batch system
echo "export GITCONDDBPATH=${RICH_BASE_GITCONDDB}" >>start_job.sh
echo "export RICH_BASE_SOFTWARE=${RICH_BASE_SOFTWARE}" >>start_job.sh
echo "export RICH_BASE_OPTIONS=${RICH_BASE_OPTIONS}" >>start_job.sh
echo "export RICH_BASE_SCRIPTS=${RICH_BASE_SCRIPTS}" >>start_job.sh

echo "" >>start_job.sh

#Gauss

if [[ $RUN_GAUSS == "1" ]]; then

    echo "cd "'$TMPDIR' >>start_job.sh
    echo "${RUN_COMMAND_GAUSS} gaudirun.py ${OPTIONS_DIR}/Gauss/Gauss-Job_"'${1}'".py > gauss.log" >>start_job.sh
    echo "" >>start_job.sh

    #get output
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "eos cp *.sim ${OUTPUT_DIR}/Gauss/data/Gauss_"'${1}'".sim" >>start_job.sh
    echo "eos cp Gauss-Histo.root ${OUTPUT_DIR}/Gauss/root/Gauss-Histo_"'${1}'".root" >>start_job.sh
    echo "eos cp gauss.log ${OUTPUT_DIR}/Gauss/log/Gauss_"'${1}'".log" >>start_job.sh
    # echo "eos cp wavelength_R1_opt1.txt ${OUTPUT_DIR}/Gauss/log/wavelength_R1_opt1_"'${1}'".txt" >>start_job.sh
    # echo "eos cp wavelength_R1_opt2.txt ${OUTPUT_DIR}/Gauss/log/wavelength_R1_opt2_"'${1}'".txt" >>start_job.sh
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "" >>start_job.sh

fi

#Boole

if [[ $RUN_BOOLE == "1" ]]; then

    echo "cd "'$TMPDIR' >>start_job.sh
    echo "${RUN_COMMAND_BOOLE} gaudirun.py ${OPTIONS_DIR}/Boole/Boole-Job_"'${1}'".py > boole.log" >>start_job.sh
    echo "" >>start_job.sh

    #get output
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "eos cp *.digi ${OUTPUT_DIR}/Boole/data/Boole_"'${1}'".digi" >>start_job.sh
    echo "eos cp Boole-Histo.root ${OUTPUT_DIR}/Boole/root/Boole-Histo_"'${1}'".root" >>start_job.sh
    echo "eos cp boole.log ${OUTPUT_DIR}/Boole/log/Boole_"'${1}'".log" >>start_job.sh
    # echo "eos cp smartIDs.txt ${OUTPUT_DIR}/Boole/smartIDs_"'${1}'".txt" >>start_job.sh
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "" >>start_job.sh

fi

#Moore

if [[ $RUN_MOORE == "1" ]]; then

    echo "cd "'$TMPDIR' >>start_job.sh
    echo "${RUN_COMMAND_MOORE} gaudirun.py ${OPTIONS_DIR}/Moore/Moore-Job_"'${1}'".py > moore.log" >>start_job.sh
    echo "" >>start_job.sh

    #get output
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "eos cp *.dst ${OUTPUT_DIR}/Moore/data/Moore_"'${1}'".dst" >>start_job.sh
    echo "eos cp Moore-Ntuple.root ${OUTPUT_DIR}/Moore/root/Moore-Ntuple_"'${1}'".root" >>start_job.sh
    echo "eos cp Moore-Histo.root ${OUTPUT_DIR}/Moore/root/Moore-Histo_"'${1}'".root" >>start_job.sh
    echo "eos cp moore.log ${OUTPUT_DIR}/Moore/log/Moore_"'${1}'".log" >>start_job.sh
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "" >>start_job.sh

fi

#Brunel

if [[ $RUN_BRUNEL == "1" ]]; then

    echo "cd "'$TMPDIR' >>start_job.sh
    echo "${RUN_COMMAND_BRUNEL} gaudirun.py ${OPTIONS_DIR}/Brunel/Brunel-Job_"'${1}'".py > brunel.log" >>start_job.sh
    echo "" >>start_job.sh

    #get output
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "eos cp *.xdst ${OUTPUT_DIR}/Brunel/data/Brunel_"'${1}'".xdst" >>start_job.sh
    echo "eos cp Brunel-Ntuple.root ${OUTPUT_DIR}/Brunel/root/Brunel-Ntuple_"'${1}'".root" >>start_job.sh
    echo "eos cp Brunel-Histo.root ${OUTPUT_DIR}/Brunel/root/Brunel-Histo_"'${1}'".root" >>start_job.sh
    echo "eos cp brunel.log ${OUTPUT_DIR}/Brunel/log/Brunel_"'${1}'".log" >>start_job.sh
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "" >>start_job.sh

fi

#Rec

if [[ $RUN_REC == "1" ]]; then

    echo "cd "'$TMPDIR' >>start_job.sh
    echo "${RUN_COMMAND_REC} gaudirun.py ${OPTIONS_DIR}/Rec/Rec-Job_"'${1}'".py > rec.log" >>start_job.sh
    echo "" >>start_job.sh

    #get output
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "eos cp Rec-Ntuple.root ${OUTPUT_DIR}/Rec/root/Rec-Ntuple_"'${1}'".root" >>start_job.sh
    echo "eos cp Rec-Histo.root ${OUTPUT_DIR}/Rec/root/Rec-Histo_"'${1}'".root" >>start_job.sh
    echo "eos cp rec.log ${OUTPUT_DIR}/Rec/log/Rec_"'${1}'".log" >>start_job.sh
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "" >>start_job.sh

fi

if [[ $RUN_REC_1 == "1" ]]; then

    echo "cd "'$TMPDIR' >>start_job.sh
    echo "${RUN_COMMAND_REC} gaudirun.py ${OPTIONS_DIR}/Rec1/Rec-Job_"'${1}'".py > rec1.log" >>start_job.sh
    echo "" >>start_job.sh

    #get output
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "eos cp Rec1-Ntuple.root ${OUTPUT_DIR}/Rec1/root/Rec-Ntuple_"'${1}'".root" >>start_job.sh
    echo "eos cp Rec1-Histo.root ${OUTPUT_DIR}/Rec1/root/Rec-Histo_"'${1}'".root" >>start_job.sh
    echo "eos cp rec1.log ${OUTPUT_DIR}/Rec1/log/Rec_"'${1}'".log" >>start_job.sh
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "" >>start_job.sh

fi

if [[ $RUN_REC_2 == "1" ]]; then

    echo "cd "'$TMPDIR' >>start_job.sh
    echo "${RUN_COMMAND_REC} gaudirun.py ${OPTIONS_DIR}/Rec2/Rec-Job_"'${1}'".py > rec2.log" >>start_job.sh
    echo "" >>start_job.sh

    #get output
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "eos cp Rec2-Ntuple.root ${OUTPUT_DIR}/Rec2/root/Rec-Ntuple_"'${1}'".root" >>start_job.sh
    echo "eos cp Rec2-Histo.root ${OUTPUT_DIR}/Rec2/root/Rec-Histo_"'${1}'".root" >>start_job.sh
    echo "eos cp rec2.log ${OUTPUT_DIR}/Rec2/log/Rec_"'${1}'".log" >>start_job.sh
    echo "sleep ${SLEEP_TIME}" >>start_job.sh
    echo "" >>start_job.sh

fi

###############submit job
echo "Submitting job ${JOB_NAME} to a queue"

chmod u+x start_job.sh
condor_submit condor.sub
