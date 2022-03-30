#!/bin/sh

# check if RICH environment is set (needs a known host)
if [ -z ${RICH_HOST+x} ]; then
    echo -e "\nRICH environment not set. Exitting.\n"
    exit 1
fi

######################################################## BEGIN CONFIG ###################
#replace scripts with local GlobalReco copy if needed
#default: draw current folder VS reference

#flag if the files are prepared with a particle gun (if info needed for particular plots)
IS_PARTICLE_GUN=false
############################################################ END OF CONFIG #######################

##############create dirs on eos
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

INPUT_DIR=${RICH_DATA}${JOB_NAME} #no / on purpose, works!
SUBMIT_DIR=$PWD
OUTPUT_DIR=$SUBMIT_DIR/output

if [ ! -e $INPUT_DIR ]; then
    echo "Directory doesn't exists. Run job to create data first:"
    echo "$INPUT_DIR"
    exit
fi

#create log file
rm -f ${SUBMIT_DIR}/plots.log
touch ${SUBMIT_DIR}/plots.log

#merge outputs
# eos rm ${INPUT_DIR}/Gauss/Gauss-Histo.root
# ${RICH_ROOT} hadd -n 0 -ff -k ${EOS_PREFIX}${INPUT_DIR}/Gauss/Gauss-Histo.root ${INPUT_DIR}/Gauss/root/Gauss-Histo_*.root >>plots.log
# eos rm ${INPUT_DIR}/Boole/Boole-Histo.root
# ${RICH_ROOT} hadd -n 0 -ff -k ${EOS_PREFIX}${INPUT_DIR}/Boole/Boole-Histo.root ${INPUT_DIR}/Boole/root/Boole-Histo_*.root >>plots.log
# eos rm ${INPUT_DIR}/Brunel/Brunel-Histo.root
# ${RICH_ROOT} hadd -n 0 -ff -k ${EOS_PREFIX}${INPUT_DIR}/Brunel/Brunel-Histo.root ${INPUT_DIR}/Brunel/root/Brunel-Histo_*.root >>plots.log
# eos rm ${INPUT_DIR}/Brunel/Brunel-Ntuple.root
# ${RICH_ROOT} hadd -n 0 -ff -k ${EOS_PREFIX}${INPUT_DIR}/Brunel/Brunel-Ntuple.root ${INPUT_DIR}/Brunel/root/Brunel-Ntuple_*.root >>plots.log
# eos rm ${INPUT_DIR}/Rec/Rec-Histo.root
# ${RICH_ROOT} hadd -n 0 -ff -k ${EOS_PREFIX}${INPUT_DIR}/Rec/Rec-Histo.root ${INPUT_DIR}/Rec/root/Rec-Histo_*.root >>plots.log
# eos rm ${INPUT_DIR}/Rec/Rec-Ntuple.root
# ${RICH_ROOT} hadd -n 0 -ff -k ${EOS_PREFIX}${INPUT_DIR}/Rec/Rec-Ntuple.root ${INPUT_DIR}/Rec/root/Rec-Ntuple_*.root >>plots.log

#create plots
rm -rf ${OUTPUT_DIR}
for OUTPUT_TYPE in simInputs occupancy performance pdResponse PID/Brunel PID/Rec1 PID/Rec2; do
    mkdir -p ${OUTPUT_DIR}/${OUTPUT_TYPE}
done

CURRENT_DIR=${INPUT_DIR}
REFERENCE_DIR=${CURRENT_DIR}
REFERENCE_JOB=${RICH_BASE_JOBS}/$(echo $REFERENCE_DIR | sed -e "s:${RICH_DATA}/::")

echo ""
echo "Creating plots for : ${CURRENT_DIR}"
echo "Reference          : ${REFERENCE_DIR}"
echo ""

# Gauss-Histo
if [ -f ${CURRENT_DIR}/Gauss/Gauss-Histo.root ]; then
    cd ${OUTPUT_DIR}/simInputs
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/Gauss/DrawSimInputs.C(\"${CURRENT_DIR}/Gauss\",\"Gauss-Histo.root\")" >>${SUBMIT_DIR}/plots.log
    cd ${OUTPUT_DIR}/occupancy
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/Gauss/DrawOccupancy.C(\"${CURRENT_DIR}/Gauss\",\"Gauss-Histo.root\")" >>${SUBMIT_DIR}/plots.log
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/Gauss/DrawOccupancyRatio.C(\"${OUTPUT_DIR}/occupancy/output.root\",\"${REFERENCE_JOB}/output/occupancy/output.root\")" >>${SUBMIT_DIR}/plots.log
    cd ${OUTPUT_DIR}/performance
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/Gauss/DrawPerformance.C(\"${CURRENT_DIR}/Gauss\",\"Gauss-Histo.root\",${IS_PARTICLE_GUN})" >>${SUBMIT_DIR}/plots.log
fi

# Boole-Histo
if [ -f ${CURRENT_DIR}/Boole/Boole-Histo.root ]; then
    cd ${OUTPUT_DIR}/occupancy
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/Boole/DrawOccupancy.C(\"${CURRENT_DIR}/Boole\")" >>${SUBMIT_DIR}/plots.log
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/Boole/DrawOccupancyRatio.C(\"${OUTPUT_DIR}/occupancy/output.root\",\"${REFERENCE_JOB}/output/occupancy/output.root\")" >>${SUBMIT_DIR}/plots.log
    # ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/Boole/DrawSinOccupancyProfile.C(\"${CURRENT_DIR}/Boole\",\"${REFERENCE_DIR}/Boole\")" >>${SUBMIT_DIR}/plots.log
    cd ${OUTPUT_DIR}/pdResponse
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/Boole/DrawPdResponse.C(\"${CURRENT_DIR}/Boole\",\"Boole-Histo.root\")" >>${SUBMIT_DIR}/plots.log
fi

# Moore-Histo
if [ -f ${CURRENT_DIR}/Moore/Moore-Histo.root ]; then
    cd ${OUTPUT_DIR}/performance
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/PID/DrawPerformance.C(\"${CURRENT_DIR}/Moore\",\"Moore-Histo.root\")" >>${SUBMIT_DIR}/plots.log
fi

# Brunel-Histo
if [ -f ${CURRENT_DIR}/Brunel/Brunel-Histo.root ]; then
    cd ${OUTPUT_DIR}/performance
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS}/output/PID/DrawPerformance.C(\"${CURRENT_DIR}/Brunel\",\"Brunel-Histo.root\")" >>${SUBMIT_DIR}/plots.log
fi

# Brunel-Ntuple
if [ -f ${CURRENT_DIR}/Brunel/Brunel-Ntuple.root ]; then
    cd ${OUTPUT_DIR}/PID/Brunel
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/MakeRichPlots.C(\"${CURRENT_DIR}/Brunel/Brunel-Ntuple.root\")" >>${SUBMIT_DIR}/plots.log
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}/Brunel/Brunel-Ntuple.root\",\"${REFERENCE_DIR}/Brunel/Brunel-Ntuple.root\")" >>${SUBMIT_DIR}/plots.log
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}/Brunel/Brunel-Ntuple.root\",\"${REFERENCE_DIR}/Brunel/Brunel-Ntuple.root\",1)" >>${SUBMIT_DIR}/plots.log
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}/Brunel/Brunel-Ntuple.root\",\"${REFERENCE_DIR}/Brunel/Brunel-Ntuple.root\",2)" >>${SUBMIT_DIR}/plots.log
fi

# Rec-Ntuple (Rich1)
if [ -f ${CURRENT_DIR}/Rec1/Rec-Ntuple.root ]; then
    cd ${OUTPUT_DIR}/PID/Rec1
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}/Rec1/Rec-Ntuple.root\",\"${REFERENCE_DIR}/Rec1/Rec-Ntuple.root\")" >>${SUBMIT_DIR}/plots.log
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}/Rec1/Rec-Ntuple.root\",\"${REFERENCE_DIR}/Rec1/Rec-Ntuple.root\",1)" >>${SUBMIT_DIR}/plots.log
fi

# Rec-Ntuple (Rich2)
if [ -f ${CURRENT_DIR}/Rec2/Rec-Ntuple.root ]; then
    cd ${OUTPUT_DIR}/PID/Rec2
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}/Rec2/Rec-Ntuple.root\",\"${REFERENCE_DIR}/Rec2/Rec-Ntuple.root\")" >>${SUBMIT_DIR}/plots.log
    ${RICH_ROOT} root -l -q -b "${RICH_BASE_SCRIPTS_GLOBAL_RECO}/RichKaonIDCompareFiles.C(\"${CURRENT_DIR}/Rec2/Rec-Ntuple.root\",\"${REFERENCE_DIR}/Rec2/Rec-Ntuple.root\",2)" >>${SUBMIT_DIR}/plots.log
fi
