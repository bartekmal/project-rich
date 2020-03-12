#!/bin/sh

#load RICH environment
source $RICH_BASE/setupRICH.sh

######################################################## BEGIN CONFIG ###################
#Scripts in GlobalReco from Rec/8ae078efadd68927f1a34ccdfafd3340b6fbcd0f
#-> GlobalPID.C: removed cut on trackLikelihood
#-> MakeRichPlots and RichKaonID modified to take dir as argument 

#replace scripts with local GlobalReco copy if needed
#default: draw current folder VS reference
############################################################ END OF CONFIG #######################

##############create dirs on eos
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

INPUT_DIR=${RICH_DATA}${JOB_NAME} #no / on purpose, works!
SUBMIT_DIR=$PWD
OUTPUT_DIR=$SUBMIT_DIR/output

if [ ! -e $INPUT_DIR ]; then
    echo "Directory doesn't exists. Run job to create data first:"
    echo "$INPUT_DIR"
    exit
fi

#create log file
rm ${SUBMIT_DIR}/plots.log; touch ${SUBMIT_DIR}/plots.log

#merge outputs
rm ${INPUT_DIR}/Gauss/Gauss-Histo.root
lb-run -c ${CMTCONFIG_ROOT} ROOT hadd -n 0 -ff -k ${INPUT_DIR}/Gauss/Gauss-Histo.root ${INPUT_DIR}/Gauss/root/Gauss_*.root >> plots.log
#rm ${INPUT_DIR}/Boole/Boole-Histo.root
#lb-run -c ${CMTCONFIG_ROOT} ROOT hadd -n 0 -ff -k ${INPUT_DIR}/Boole/Boole-Histo.root ${INPUT_DIR}/Boole/root/Boole_*.root >> plots.log
#rm ${INPUT_DIR}/Brunel/Brunel-Histo.root
#lb-run -c ${CMTCONFIG_ROOT} ROOT hadd -n 0 -ff -k ${INPUT_DIR}/Brunel/Brunel-Histo.root ${INPUT_DIR}/Brunel/root/Brunel-Histo_*.root >> plots.log
rm ${INPUT_DIR}/Brunel/Brunel-Ntuple.root
lb-run -c ${CMTCONFIG_ROOT} ROOT hadd -n 0 -ff -k  ${INPUT_DIR}/Brunel/Brunel-Ntuple.root ${INPUT_DIR}/Brunel/root/Brunel-Ntuple_*.root >> plots.log

#create plots
rm -rf ${OUTPUT_DIR}; mkdir ${OUTPUT_DIR}; cd ${OUTPUT_DIR}

lb-run -c ${CMTCONFIG_ROOT} ROOT root -l -q -b "${RICH_BASE_SCRIPTS}/output/Gauss/DrawOccupancy.C(\"${INPUT_DIR}/Gauss\")" >> ${SUBMIT_DIR}/plots.log

CURRENT_DIR=${INPUT_DIR}/Brunel
REFERENCE_DIR=${RICH_DATA}/main/Brunel
#REFERENCE_DIR=${RICH_DATA}/main/dddb-20190726/bEvent/Brunel
#REFERENCE_DIR=${RICH_DATA}/main/dddb-20190223/bEvent/Brunel

echo ""
echo "Creating plots for : ${CURRENT_DIR}"
echo "Reference          : ${REFERENCE_DIR}"
echo ""

lb-run -c ${CMTCONFIG_ROOT} ROOT root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/MakeRichPlots.C(\"${CURRENT_DIR}\")" >> ${SUBMIT_DIR}/plots.log

lb-run -c ${CMTCONFIG_ROOT} ROOT root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}\",\"${REFERENCE_DIR}\")" >> ${SUBMIT_DIR}/plots.log
lb-run -c ${CMTCONFIG_ROOT} ROOT root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}\",\"${REFERENCE_DIR}\",1)" >> ${SUBMIT_DIR}/plots.log
lb-run -c ${CMTCONFIG_ROOT} ROOT root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}\",\"${REFERENCE_DIR}\",2)" >> ${SUBMIT_DIR}/plots.log
#lb-run -c ${CMTCONFIG_ROOT} ROOT root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}\",\"${REFERENCE_DIR}\",3)" >> ${SUBMIT_DIR}/plots.log
