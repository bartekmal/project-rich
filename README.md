# General info
Project for various scripts & tools for RICH upgrade studies & maintenance.

## Prerequisites

* LHCb environment (lb-* scripts / Python3 / ROOT / CVMFS)
* CERN EOS access

**The setup described [below](https://gitlab.cern.ch/bmalecki/rich-upgrade#getting-started) should work 'out of the box' (currently) only on lxplus. The repository must be somewhere in the user `/afs/cern.ch/work` area. This can be improved/adapted in the setup script `setup.sh`.**

## Submodules
* [Alexandria](https://gitlab.cern.ch/lhcb/Alexandria) package for LHCb styles management (used in `${RICH_BASE_SCRIPTS_UTILS}/Styles.C`)

## Directories

* `common`: tools for running typical tasks
    * `options`: RICH-specific options for LHCb applications
    * `scripts`: scripts for performing typical RICH performance studies and tasks (e.g. updating LHCb databases)
        * `Makefile` / `run_Job.py` / `run_Plots.sh`: template scripts for running RICH studies (managed via the `Makefile` - see [here](https://gitlab.cern.ch/bmalecki/rich-upgrade#getting-started))
        * `output`: scripts for analysing simulation output
        * `utils`: helper classes/modules (common for other scripts) 
* `data`: space for any input files (not git-tracked by default)
* `env`: local Python virtual environment
* `jobs`: directory for running RICH studies (some dedicated subdirectories propsed below)
    * `reference`: characteristic configuration (e.g. baseline performance, spillover studies)
    * `feature`: test jobs when adding a new feature
    * `study`: more 'physical' studies (e.g. physics performance with various detector configurations)
* `misc`: space for misc files (e.g. documentation) (not git-tracked by default)
* `software`: software 'external' to the RICH-specific scripts (e.g. LHCb software)
    * `data`: LHCb databases
    * `dedicated`: LHCb stacks dedicated to particular studies/developments
    * `stack-lhcb`: default LHCb stack
    * `stack-gauss`: default Gauss build
    * `utils`: other useful packages

# Getting started
It is assumed that you will deveoping the RICH simulation / studies, hence local builds of the LHCb software/databases are needed.

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
In general, follow the instructions in [this project](https://gitlab.cern.ch/rmatev/lb-stack-setup). Examples of how to build the required software for the current repository are given below (see the instructions mentioned above in case of issues). Alternative ways to access/build the LHCb software may be used as well, e.g.:
```
lb-run Boole
```
```
lb-run --nightly=lhcb-head Boole
```
```
lb-dev Boole
```

These are only examples, see the detailed documentation [here](https://lhcb-core-doc.web.cern.ch/lhcb-core-doc/index.html). In case of manual building of the stack, useful information can be found in the [nightlies](https://lhcb-nightlies.web.cern.ch/nightly/) (current LHCb stack status) and in the individual project repositories - example [here](https://gitlab.cern.ch/lhcb/Gauss) (links to documentation, details on the dependencies used in particular tags/releases).

Prepare the required default LHCb stack in `stack-lhcb/`:
```
cd ${RICH_BASE_SOFTWARE}
curl https://gitlab.cern.ch/rmatev/lb-stack-setup/raw/master/setup.py | python3 - stack-lhcb
cd stack-lhcb

make
```

Prepare the required default Gauss build in `stack-gauss/`:
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

## Quick start / running RICH studies

**Remember to run `source setup.sh` each time you start working with this repository in a new terminal etc.**

**Your first study should be named as `reference/baseline` and - in principle - correspond to the current baseline configuration. Any other created study will by default look for this directory as a reference (some output analysis scripts create ratios of respective histograms between the current/referece studies to enable quick comparisons).**

 **The mentioned baseline configuration will be used by default, but various other types of studies are already defined in the `Makefile` and can be configured there.**

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

Once the jobs are finished (you can make sure that the output files exist by calling `make data-show-list | less`), you can run analysis scripts that will produce a set of results in the `output` area:
```
make merge; make plots
```

# Important files

## `setup.sh`
* defines a set of useful aliases/functions within `rich-*` 'namespace'
* defines the current workspace (see `$RICH_BASE*` variables)

## `common/scripts/Makefile` (~analysis pipeline)

**This script is copied to any new study folder while creating a study by `rich-make-study`, which enables local configuration.**

* defines the default analysis pipeline (baseline RICH performance study)
* many other dedicated studies are also defined (please get in touch if you have troubles using it - many types of studies can be managed from here adapting the configuration slightly)

**Scripts `run_Job.sh` and `run_Plots.sh` are generated in individual jobs folders when calling `make jobs`. Running the analysis should be managed via `Makefile`, but these scripts can also be modified locally if needed in specific cases (e.g. to disable a part of the simulation chain).**

# Contributing

1. Once you have the cloned repository, just do the following in the main folder:

`git checkout master`

`git pull`

`git checkout -b newBranch`

`git push --set-upstream origin newBranch`

2. Then you can normally add/commit/push to the new branch.

**Please get in touch before merging to master (for example bartosz.piotr.malecki@cern.ch).**

