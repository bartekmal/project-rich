# Project structure

## Directories

* `common`: tools for running typical tasks
    * `options`: RICH-specific options for LHCb applications
    * `scripts`: scripts for performing typical RICH performance studies and tasks (e.g. updating LHCb databases)
        * `Makefile` / `run_Job.py` / `run_Plots.sh`: template scripts for running RICH studies (managed via the `Makefile`)
        * `output`: scripts for analysing simulation output
        * `utils`: helper classes/modules (common for other scripts) 
* `env`: local Python virtual environment
* `jobs`: directory for running RICH studies (some dedicated subdirectories propsed below)
    * `reference`: characteristic configuration (e.g. baseline performance, spillover studies)
    * `feature`: test jobs when adding a new feature
    * `study`: more 'physical' studies (e.g. physics performance with various detector configurations)
* `software`: software 'external' to the RICH-specific scripts (e.g. LHCb software)
    * `data`: LHCb databases
    * `dedicated`: LHCb stacks dedicated to particular studies/developments
    * `stack-lhcb`: default LHCb stack
    * `stack-gauss`: default Gauss build
    * `utils`: other useful packages

## Important files

### `setup.sh`
* defines a set of useful aliases/functions within `rich-*` 'namespace'
* defines the current workspace (see `$RICH_BASE*` variables)

### `common/scripts/Makefile` (~analysis pipeline)

**This script is copied to any new study folder while creating a study by `rich-make-study`, which enables local configuration.**

* defines the default analysis pipeline (baseline RICH performance study)
* many other dedicated studies are also defined (please get in touch if you have troubles using it - many types of studies can be managed from here adapting the configuration slightly)

**Scripts `run_Job.sh` and `run_Plots.sh` are generated in individual jobs folders when calling `make jobs`. Running the analysis should be managed via `Makefile`, but these scripts can also be modified locally if needed in specific cases (e.g. to disable a part of the simulation chain).**
