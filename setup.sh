#!/bin/sh
# Set up useful paths/aliases for the project.
# Needs to be called from the main repository area.

# functions
function rich-setup-env() {
  export PS1="[\u@\h \W] (RICH) \$ "

  #useful dirs
  export RICH_BASE_SOFTWARE=${RICH_BASE}/software
  export RICH_BASE_GITCONDDB=${RICH_BASE_SOFTWARE}/data
  export RICH_BASE_SOFTWARE_STACK=${RICH_BASE_SOFTWARE}/stack-lhcb
  export RICH_BASE_SOFTWARE_GAUSS=${RICH_BASE_SOFTWARE}/stack-gauss
  export RICH_BASE_OPTIONS=${RICH_BASE}/common/options
  export RICH_BASE_SCRIPTS=${RICH_BASE}/common/scripts
  export RICH_BASE_SCRIPTS_ANALYSIS=${RICH_BASE_SCRIPTS}/output
  export RICH_BASE_SCRIPTS_UTILS=${RICH_BASE_SCRIPTS}/utils
  export RICH_BASE_SCRIPTS_GLOBAL_RECO=${RICH_BASE_SOFTWARE}/stack-lhcb/Rec/Rec/GlobalReco/root
  export RICH_BASE_JOBS=${RICH_BASE}/jobs

  # common variables (needed for running jobs/scripts etc)
  export EOS_PREFIX=""
  export SLEEP_TIME=5 # after finishing one application, before using its output
  export RICH_COMPUTING_GROUP=""

  # software versions  
  export CMTCONFIG_GAUSS=x86_64_v2-centos7-gcc11-opt
  export CMTCONFIG_STACK=x86_64_v2-centos7-gcc11-opt

  lb-set-platform ${CMTCONFIG_STACK}
  lb-set-workspace ${RICH_BASE_SOFTWARE}/stack-lhcb

  # activate local python env
  # ! needs preparing a local Python virtual environment
  export RICH_PYTHON=${RICH_BASE}/env/bin/activate
  source ${RICH_PYTHON}

  # fixed ROOT version
  # TODO improve, e.g. use "lb-conda default"
  export RICH_ROOT="/cvmfs/lhcb.cern.ch/lib/var/lib/LbEnv/2068/stable/linux-64/bin/lb-run -c x86_64-centos7-gcc9-opt -p LD_LIBRARY_PATH=/cvmfs/lhcb.cern.ch/lib/lcg/releases/gcc/9.2.0-afc57/x86_64-centos7/lib64/ ROOT"

  #useful aliases
  alias rich-cd-base="cd ${RICH_BASE}"
  alias rich-cd-data="cd ${RICH_DATA}"
  alias rich-cd-software="cd ${RICH_BASE_SOFTWARE}"
  alias rich-cd-options="cd ${RICH_BASE_OPTIONS}"
  alias rich-cd-scripts="cd ${RICH_BASE_SCRIPTS}"
  alias rich-cd-jobs="cd ${RICH_BASE_JOBS}"
  alias rich-root="${RICH_ROOT}"
}

function rich-setup-summary() {
  echo -e "\nRICH environment for ${RICH_HOST} ready!"
  echo "--> project path: ${RICH_BASE}"
  echo "--> data path: ${RICH_DATA}"
  echo "--> platform: ${CMTCONFIG}"
  echo "--> Python: ${RICH_PYTHON}"
  echo -e "--> ROOT: ${RICH_ROOT} \n"
}

function rich-setup-noUserArea-gauss() {
  echo ""
  echo "--> Setting up RICH with no UserArea for Gauss"
  echo ""
  export PS1="[\u@\h \W] (no UserArea - Gauss) \$ "
  lb-set-platform ${CMTCONFIG_GAUSS}
  lb-set-workspace
  rich-setup-summary
}

function rich-setup-noUserArea() {
  echo ""
  echo "--> Setting up RICH with no UserArea"
  echo ""
  export PS1="[\u@\h \W] (no UserArea) \$ "
  lb-set-workspace
  rich-setup-summary
}

function rich-setup-localWorkspace() {
  echo ""
  echo "--> Setting up RICH with local workspace"
  echo ""

  NEW_SOFTWARE_DIR=${PWD}

  export PS1="[\u@\h \W] (local workspace) \$ "
  lb-set-workspace ${NEW_SOFTWARE_DIR}
  rich-setup-summary
}

function rich-setup-gaussStack() {
  echo ""
  echo "--> Setting up Gauss stack sub-environment."
  echo ""

  NEW_SOFTWARE_DIR=${RICH_BASE_SOFTWARE_GAUSS}

  export PS1="[\u@\h \W] (Gauss - stack) \$ "
  lb-set-platform ${CMTCONFIG_GAUSS}
  lb-set-workspace ${NEW_SOFTWARE_DIR}
  rich-setup-summary
}

function rich-make-study() {
  # config
  STUDY_PATH=${RICH_BASE_JOBS}/${1:-test}

  # sanitise
  if [ -d ${STUDY_PATH} ]; then
    echo "Directory already exists:"
    echo ${STUDY_PATH}
    return 1
  else
    echo "Creating a new study in:"
    echo ${STUDY_PATH}
    echo -e "\nYou can now configure your study in the local Makefile.\n"
    mkdir -p ${STUDY_PATH}
    cp ${RICH_BASE_SCRIPTS}/Makefile ${STUDY_PATH}
    cd ${STUDY_PATH}
  fi
}

function rich-make-dd4hep-test() {
  # config
  STUDY_PATH=${RICH_BASE_JOBS}/${1:-test-dd4hep}

  # sanitise
  if [ -d ${STUDY_PATH} ]; then
    echo "Directory already exists:"
    echo ${STUDY_PATH}
    return 1
  else
    echo "Creating a new DD4HEP test area in:"
    echo ${STUDY_PATH}
    mkdir -p ${STUDY_PATH}
    cp ${RICH_BASE_SCRIPTS}/dd4hep/tests/run.sh ${STUDY_PATH}
    cp ${RICH_BASE_SCRIPTS}/dd4hep/tests/grep.sh ${STUDY_PATH}
    cd ${STUDY_PATH}
  fi
}

function rich-run-panoramix() {
  lb-run -c x86_64-slc6-gcc62-opt Panoramix/v24r1 python '$myPanoramix' --BareConfig 1 -f none -v $1
}

# setup - default

# get LHCb environment (should also provide access to general needed software, e.g. Python) 
# TODO could be improved by providing a fixed software container
source /cvmfs/lhcb.cern.ch/lib/LbEnv.sh # ! can be deactivated if available by default

# host-specific configuration
# ! define a host-specific 'workspace' (it depends on the available storage 'resources')
export RICH_HOST=0

# ! lxplus: repository needs to reside in the user /afs/cern.ch/work area and use EOS for data storage (see below)
if [[ $(hostname) = "lxplus"*".cern.ch" ]]; then
    export RICH_HOST=lxplus

    # ! configure host-specific working and data storage directories
    WORK_DIR=/afs/cern.ch/work
    DATA_DIR=/eos/lhcb/user

    # get whatever comes after USER name/path in the directory where this scripts resides (~relative path wrt 'user area')
    USER_PATH=${USER:0:1}/${USER}
    RICH_SETUP_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
    RICH_BASE_FOLDER=$(echo ${RICH_SETUP_DIR} | sed -n -e 's/^.*'${USER}'\///p')

    # basic directories (absolute paths)
    export RICH_BASE=${WORK_DIR}/${USER_PATH}/${RICH_BASE_FOLDER}
    export RICH_DATA=${DATA_DIR}/${USER_PATH}/${RICH_BASE_FOLDER}
fi

# check if RICH environment is set (needs a known host)
if [ -z ${RICH_HOST+x} ]; then
  echo -e "\nRICH environment not set. Exitting.\n"
  exit 1
fi

# RICH-specific setup
rich-setup-env
rich-setup-summary