# General info
Project for various scripts & tools for RICH upgrade studies & maintenance.

Details on the **project structure and configuration options** are given [here](doc/projectStructure.md).

Documentation on the **RICH simulation** (details on the various simulation components + intructions for typical tasks) is provided [here](doc/richSimulation.md).

Instructions for some **typical tasks** are provided [here](doc/typicalTasks.md/).

Feel free to [contribute](CONTRIBUTING.md).

## Prerequisites

* LHCb environment (lb-* scripts / Python3 / ROOT / CVMFS)
* CERN EOS access

**The setup described [below](#getting-started) should work 'out of the box' (currently) only on lxplus. The repository must be somewhere in the user `/afs/cern.ch/work` area. This can be improved/adapted in the setup script `setup.sh`.**

## Submodules
* [Alexandria](https://gitlab.cern.ch/lhcb/Alexandria) package for LHCb styles management (used in `${RICH_BASE_SCRIPTS_UTILS}/Styles.C`)

# Getting started
It is assumed that you will deveoping the RICH simulation / studies, hence local builds of the LHCb software/databases are needed.

**Consider using an `ssh-agent` to authenticate to GitLab (see details [here](https://docs.gitlab.com/ee/user/ssh.html)). Otherwise the execution of instructions below will break at each password typing and following them will be much less convenient. If you are using a rather default configuration of your SSH keys, the lines below might be enough. See more details on Git configuration for LHCb [here](https://lhcb.github.io/starterkit-lessons/second-analysis-steps/lb-git.html).**

```
eval $(ssh-agent -s)
ssh-add <directory to private SSH key>
```

## Initialising the repository
Set up the repository (in your user `/afs/cern.ch/work` area on lxplus) and prepare a local Python virtual environment in `env/`:
```
git clone --recurse-submodules ssh://git@gitlab.cern.ch:7999/bmalecki/rich-upgrade.git
cd rich-upgrade

python -m venv env
source env/bin/activate
pip install parse

source setup.sh
```

**The `setup.sh` script should be sourced each time you start working with this repository in a new terminal etc.**

Get LHCb databases:
```
git clone -b upgrade/master ssh://git@gitlab.cern.ch:7999/lhcb-conddb/DDDB.git ${RICH_BASE_GITCONDDB}/DDDB
git clone -b upgrade/master ssh://git@gitlab.cern.ch:7999/lhcb-conddb/SIMCOND.git ${RICH_BASE_GITCONDDB}/SIMCOND
git clone ssh://git@gitlab.cern.ch:7999/lhcb-conddb/lhcb-conditions-database.git ${RICH_BASE_GITCONDDB}/lhcb-conditions-database
```

## Building LHCb software
In general, follow the instructions in [this project](https://gitlab.cern.ch/rmatev/lb-stack-setup). Examples on how to **build the required software** for the current repository are given below (see the instructions mentioned above in case of issues or more details and useful links [here](doc/lhcbSoftware.md)). 

Prepare the required default LHCb stack in `stack-lhcb/`:
```
cd ${RICH_BASE_SOFTWARE}
curl https://gitlab.cern.ch/rmatev/lb-stack-setup/raw/master/setup.py | python3 - stack-lhcb
cd stack-lhcb

make Boole Moore
```

Prepare the required default Gauss build in `stack-gauss/` (see details on Gauss releases [here](https://lhcbdoc.web.cern.ch/lhcbdoc/gauss/releases/) or [here](https://gitlab.cern.ch/lhcb/Gauss/-/releases)). The first version below is the simplest one (just for running a released Gauss version). For Gauss development, the second one might be a better solution. If you have issues with any of these versions, consider [contacting the simulation experts](https://mattermost.web.cern.ch/lhcb/pl/hdbenhrm8byqfd9zmcgcgsae9a).
```
cd ${RICH_BASE_SOFTWARE}
mkdir stack-gauss; cd stack-gauss

rich-setup-noUserArea-gauss

lb-dev Gauss/v56r2 --name Gauss # ! choose required (? latest) version
cd Gauss
git lb-use Gauss

make install
```

```
cd ${RICH_BASE_SOFTWARE}
curl https://gitlab.cern.ch/rmatev/lb-stack-setup/raw/master/setup.py | python3 - stack-gauss
cd stack-gauss

make fast/Gauss/checkout  # clone Gauss if not already there
git -C Gauss fetch ssh://git@gitlab.cern.ch:7999/lhcb-nightlies/Gauss.git lhcb-gauss-dev
git -C Gauss checkout FETCH_HEAD

make fast/Gauss
```

**(optional)** Add dedicated branches (rebasing to master and rebuilding relevant projects may be needed):
* Gauss:
    * bmalecki-rich-setup-monitoring
    * bmalecki-rich-setup-simInput
* Rec:
    * bmalecki-rich-setup-reco

## First RICH study

**Remember to run `source setup.sh` each time you start working with this repository in a new terminal etc.**

**Your first study should be named as `reference/baseline` and - in principle - correspond to the current baseline configuration. Any other created study will by default look for this directory as a reference (some output analysis scripts create ratios of respective histograms between the current/referece studies to enable quick comparisons).**

**The mentioned baseline configuration will be used by default, but various other types of studies are already defined in the `Makefile` and can be configured there (see [here](doc/projectStructure.md) and [here](doc/richSimulation.md/#digitisation)).**

Start with the steps below (from the main directory) to run the LHCb simulation configured for the RICH studies. This step is performed using the CERN batch system and can take several hours (depending on the specific study configuration and the availalbe resources). You can check the jobs status using `condor_q`.
```
source setup.sh
rich-make-study reference/baseline
make jobs
```

**(optional)** You can run a simple local test to see if your simulation chain works properly:
```
cd test
./run_Job.sh
tmp/start_job.sh
./run_Plots.sh
```

To submit the jobs to the batch system do:
```
make run
```

Once the jobs are finished (you can make sure that the output files exist by calling `make data-show-list | less`), you can run analysis scripts that will produce a set of results in the `output` area (see [here](doc/richSimulation.md/#performance-studies) for the meaning of the results):
```
make merge; make plots
```
