# Typical tasks

Scripts for various typical tasks are available [here](common/scripts).

## Updating conditions databases

Conditions in the XML format ([SIMCOND](https://gitlab.cern.ch/lhcb-conddb/SIMCOND)) are created with [this](common/scripts/SIMCOND/makePdProperties.py) script. They need to be copied from the `output/` folder to your required directory with `SIMCOND` database (should be `software/data/SIMCOND` by default). These conditions can be parsed to the YAML format ([lhcb-conditions-database](https://gitlab.cern.ch/lhcb-conddb/lhcb-conditions-database)) with [this](common/scripts/lhcb-conddb/parsePdPropertiesXmlToYaml.py) script. You need to provide a path to the XML conditions version (looking in the `software/data/SIMCOND` directory by default). Again, the `output/` folder needs to be copied to your required directory with `lhcb-conditions-database` database.

**The `output/` folders are temporary files and are not git-tracked. You may need to remove them manually upon warning that the directory already exists.**

Recreating the final conditions in the XML format requires some input data. This information, together with necessary processing scripts is [provided](common/scripts/SIMCOND/input/) in the repository. Following the evolution of some the properties, you may need to provide new input in the directory mentioned above and update paths to input files in the main script for conditions creation. If the latter displays warning on missing input files, you may need to run relevant processing scripts for the given input type (see example below).

For example, [this](common/scripts/SIMCOND/input/occupancy) directory contains information on hit-level occupancy that is used as input for the SIN simulation. **Currently, this information is based on the simulation (this may be changed to use the input from data in the future).** If this needs to be updated (for example after changes of the simulation version or geometry description), you need to provide new files in relevant format (see instructions below and examples of the folder structure within the `common/scripts/SIMCOND/input/occupancy` folder), update paths in the [processing script](common/scripts/SIMCOND/input/occupancy/getOccupancy.py), run it, and then update the paths (search for phrase `configList`) and run the [main script](common/scripts/SIMCOND/makePdProperties.py) for conditions creation.

The required input files in this case are `occupancyPd_Gauss_R1.C` and `occupancyPd_Gauss_R2.C` from the `output/occupancy` folder in the `bEvent_basic` job for the new simulation configuration. These files can be produced via `make plots` after temporarily uncommenting the lines containing phrase `SaveAs(".C")` in the script `common/scripts/output/Gauss/DrawOccupancy.C`.
