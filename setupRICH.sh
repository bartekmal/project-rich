#!/bin/sh

####################### functions ######################

function rich_run_panoramix {
  lb-run -c x86_64-slc6-gcc62-opt Panoramix/v24r1 python '$myPanoramix' --BareConfig 1 -f none -v $1
}
function rich_run_panoramix_localDB {
  lb-run -c x86_64-slc6-gcc62-opt -s GITCONDDBPATH=$RICH_BASE_SOFTWARE Panoramix/v24r1 python '$myPanoramix' --BareConfig 1 -f none -v $1
}

function rich_run_gauss_localDB {
  $RICH_BASE_SOFTWARE/stack_Gauss/Gauss/$CMTCONFIG_GAUSS/run -s GITCONDDBPATH=$RICH_BASE_SOFTWARE gaudirun.py $RICH_BASE_OPTIONS/Gauss/Gauss-Job.py
}

function rich_setup_env {

    #common variables (for running jobs etc)
    export EOS_PREFIX="root://eoslhcb.cern.ch/"
    export SLEEP_TIME=10 #after finishing one application, before using output
    export CMTCONFIG_GAUSS=x86_64-centos7-gcc9-opt
    export CMTCONFIG_STACK=x86_64-centos7-gcc9-opt
    export CMTCONFIG_ROOT=x86_64-centos7-gcc8-opt
    
    #useful dirs
    export RICH_BASE_SOFTWARE=$RICH_BASE/software
    export RICH_BASE_OPTIONS=$RICH_BASE/common/options
    export RICH_BASE_SCRIPTS=$RICH_BASE/common/scripts
    export RICH_BASE_SCRIPTS_GLOBAL_RECO=$RICH_BASE_SCRIPTS/output/PID/GlobalReco

    #useful aliases
    alias rich_go_base="cd $RICH_BASE"
    alias rich_go_data="cd $RICH_DATA"
    alias rich_go_software="cd $RICH_BASE_SOFTWARE"
    alias rich_go_options="cd $RICH_BASE_OPTIONS"
    alias rich_go_scripts="cd $RICH_BASE_SCRIPTS"

    #login
    lb-set-platform ${CMTCONFIG_STACK}
    lb-set-workspace ${RICH_BASE_SOFTWARE}
}

function rich_setup_loginInfo {
    echo -e "\nRICH_Upgrade environment for $RICH_HOST ready!"
    echo "--> project path: $RICH_BASE"
    echo "--> data path: $RICH_DATA"
    echo -e "--> platform: $CMTCONFIG \n"
    echo -e "--> Flavour: $PS1 \n"
}


function rich_setup_noUserArea_gauss {
  echo ""
  echo "--> Setting up RICH with no UserArea for Gauss"
  echo ""
  export PS1="[\u@\h \W] (no UserArea - Gauss) \$ "
  lb-set-platform ${CMTCONFIG_GAUSS}
  lb-set-workspace
  rich_setup_loginInfo
}

function rich_setup_noUserArea {
  echo ""
  echo "--> Setting up RICH with no UserArea"
  echo ""
  export PS1="[\u@\h \W] (no UserArea) \$ "
  lb-set-workspace
  rich_setup_loginInfo
}

function rich_setup_gauss_stack {
  echo ""
  echo "--> Setting up Gauss stack sub-environment."
  echo ""

  NEW_SOFTWARE_DIR=$RICH_BASE_SOFTWARE/stack_Gauss

  export CCACHE_DIR=$NEW_SOFTWARE_DIR/.ccache
  export PS1="[\u@\h \W] (Gauss - stack) \$ "
  lb-set-platform ${CMTCONFIG_GAUSS}
  lb-set-workspace $NEW_SOFTWARE_DIR
  rich_setup_loginInfo
}

######## setupRICH - default ##############

#setup LbEnv
source /cvmfs/lhcb.cern.ch/lib/LbEnv.sh

#check host
export RICH_HOST=0
if [[ $USER = "bmalecki" ]] ; then
  export RICH_HOST=lxplus
  RICH_SETUP_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
  RICH_BASE_FOLDER=`echo $RICH_SETUP_DIR | sed -n -e 's/^.*bmalecki\///p' `
  export RICH_BASE=/afs/cern.ch/work/b/bmalecki/${RICH_BASE_FOLDER}
  export RICH_DATA=/eos/lhcb/user/b/bmalecki/${RICH_BASE_FOLDER}
fi

if [[ $USER = "plgbmalecki" ]] ; then
  export RICH_HOST=zeus
  export RICH_BASE=$STORAGE/RICH_Upgrade
  export RICH_DATA=$SCRATCH/RICH_Upgrade
fi
if [ "$RICH_HOST" = "0" ]; then
  echo -e "\nRICH_Upgrade environemnt not set! -> exiting"
  exit 1
fi

#set RICH environment variables (default setup)
rich_setup_env
rich_setup_loginInfo

