#!/bin/sh

####################### functions ######################

function rich-run-panoramix {
  lb-run -c x86_64-slc6-gcc62-opt Panoramix/v24r1 python '$myPanoramix' --BareConfig 1 -f none -v $1
}

function rich-setup-env {

    #common variables (for running jobs etc)
    export EOS_PREFIX="root://eoslhcb.cern.ch/"
    export SLEEP_TIME=10 #after finishing one application, before using output
    export CMTCONFIG_GAUSS=x86_64-centos7-gcc9-opt
    export CMTCONFIG_STACK=x86_64-centos7-gcc9-opt
    export CMTCONFIG_ROOT="x86_64-centos7-gcc9-opt -p LD_LIBRARY_PATH=/cvmfs/lhcb.cern.ch/lib/lcg/releases/gcc/9.2.0-afc57/x86_64-centos7/lib64/"

    #useful dirs
    export RICH_BASE_SOFTWARE=$RICH_BASE/software
    export RICH_BASE_OPTIONS=$RICH_BASE/common/options
    export RICH_BASE_SCRIPTS=$RICH_BASE/common/scripts
    export RICH_BASE_SCRIPTS_GLOBAL_RECO=$RICH_BASE_SCRIPTS/output/PID/GlobalReco

    #useful aliases
    alias rich-cd-base="cd $RICH_BASE"
    alias rich-cd-data="cd $RICH_DATA"
    alias rich-cd-software="cd $RICH_BASE_SOFTWARE"
    alias rich-cd-options="cd $RICH_BASE_OPTIONS"
    alias rich-cd-scripts="cd $RICH_BASE_SCRIPTS"

}

function rich-setup-loginInfo {
    echo -e "\nRICH_Upgrade environment for $RICH_HOST ready!"
    echo "--> project path: $RICH_BASE"
    echo "--> data path: $RICH_DATA"
    echo -e "--> platform: $CMTCONFIG \n"
    echo -e "--> Flavour: $PS1 \n"
}


function rich-setup-noUserArea-gauss {
  echo ""
  echo "--> Setting up RICH with no UserArea for Gauss"
  echo ""
  export PS1="[\u@\h \W] (no UserArea - Gauss) \$ "
  lb-set-platform ${CMTCONFIG_GAUSS}
  lb-set-workspace
  rich-setup-loginInfo
}

function rich-setup-noUserArea {
  echo ""
  echo "--> Setting up RICH with no UserArea"
  echo ""
  export PS1="[\u@\h \W] (no UserArea) \$ "
  lb-set-workspace
  rich-setup-loginInfo
}

function rich-setup-gaussStack {
  echo ""
  echo "--> Setting up Gauss stack sub-environment."
  echo ""

  NEW_SOFTWARE_DIR=$RICH_BASE_SOFTWARE/stack_Gauss

  export CCACHE_DIR=$NEW_SOFTWARE_DIR/.ccache
  export PS1="[\u@\h \W] (Gauss - stack) \$ "
  lb-set-platform ${CMTCONFIG_GAUSS}
  lb-set-workspace $NEW_SOFTWARE_DIR
  rich-setup-loginInfo
}

######## setupRICH - default ##############

#setup LbEnv
source /cvmfs/lhcb.cern.ch/lib/LbEnv.sh

#check host
export RICH_HOST=0
if [[ $(hostname) = "lxplus"*".cern.ch" ]] || [[ $(hostname) = "lbd"*".cern.ch" ]]; then
  export RICH_HOST=lxplus
  RICH_SETUP_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
  RICH_BASE_FOLDER=`echo $RICH_SETUP_DIR | sed -n -e 's/^.*bmalecki\///p' `
  export RICH_BASE=${MY_WORK_DIR}/${RICH_BASE_FOLDER}
  export RICH_DATA=${MY_DATA_DIR}/${RICH_BASE_FOLDER}
fi

if [ "$RICH_HOST" = "0" ]; then
  echo -e "\nRICH_Upgrade environemnt not set! -> exiting"
  exit 1
fi

#set RICH environment variables (default setup)
rich-setup-env
lb-set-platform ${CMTCONFIG_STACK}
lb-set-workspace ${RICH_BASE_SOFTWARE}
rich-setup-loginInfo



