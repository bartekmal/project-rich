#!/bin/sh

# check if RICH environment is set (needs a known host)
if [ -z ${RICH_HOST+x} ]; then
    echo -e "\nRICH environment not set. Exitting.\n"
    exit 1
fi

# run
lb-run LHCb python makeSmartIds.py
# ${RICH_BASE_SOFTWARE}/dedicated/feature-conddb/LHCb/build.${CMTCONFIG_STACK}/run python makeSmartIds.py
