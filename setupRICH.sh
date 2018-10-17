#!/bin/sh
source /cvmfs/lhcb.cern.ch/group_login.sh

#check host
export RICH_HOST=0
if [[ $(hostname) = "lxplus"[0-9][0-9][0-9]".cern.ch" ]] ; then
  export RICH_HOST=lxplus
  export RICH_BASE=/afs/cern.ch/work/b/bmalecki/RICH_Upgrade
  export RICH_DATA=/eos/lhcb/user/b/bmalecki/RICH_Upgrade
fi

if [[ $(hostname) = "zeus.cyfronet.pl" ]] ; then
  export RICH_HOST=zeus
  export RICH_BASE=$STORAGE/RICH_Upgrade
  export RICH_DATA=$SCRATCH/RICH_Upgrade
fi
if [ "$RICH_HOST" = "0" ]; then
  echo -e "\nRICH_Upgrade environemnt not set! -> exiting"
  exit 1
fi

#set LbLogin variables
export CMTCONFIG=x86_64-slc6-gcc7-opt
export User_release_area="$RICH_BASE/software"
LbLogin

echo -e "\nRICH_Upgrade environment for $RICH_HOST ready!"
echo "--> project path: $RICH_BASE"
echo "--> data path: $RICH_DATA"
echo "--> User_release_area: $User_release_area"
echo -e "--> CMTCONFIG: $CMTCONFIG \n"

#set ccache
export CCACHE_DIR=$RICH_BASE/.ccache
export CMAKEFLAGS="-DCMAKE_USE_CCACHE=ON"

#useful dirs
export RICH_BASE_SOFTWARE=$User_release_area

#useful aliases
alias rich_go_base="cd $RICH_BASE"
alias rich_go_data="cd $RICH_DATA"
alias rich_go_software="cd $RICH_BASE_SOFTWARE"




