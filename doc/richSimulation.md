# RICH simulation

General introduction to the LHCb RICH detectors, contributions to their performance, and the RICH simulation is given e.g. in [[this thesis](https://cds.cern.ch/record/2804165)] (see Sect. 3.3).

More details on the RICH digitisation are given [below](#digitisation) and in [this presentation](https://indico.cern.ch/event/1117671/contributions/4693675) (see also references within for details on the various simulation components).

RICH performance studies are described [below](#performance-studies) and in [this presentation](https://indico.cern.ch/event/985420/contributions/4149774).

**All of the links refer to the simulation status at some point in time and aim to provide a general approach/description of the given simulation component. For the current status, it is best to refer to the actual code, e.g. in the [RichReadout](https://gitlab.cern.ch/lhcb/Boole/-/tree/master/Rich/RichReadout) package.**

## Digitisation
Details on the implementation of various simulation/digitisation components and examples of performed studies are given below. 
* numbering scheme & modules layout [[initial MaPMT](https://indico.cern.ch/event/848117/contributions/3563406)][[final MaPMT](https://indico.cern.ch/event/1169516/contributions/4911839)]
* hardware properties (PD channels gains, thresholds, SIN) [[meaning of parameters](https://indico.cern.ch/event/881474/contributions/3713658)][[use of commissioning data](https://indico.cern.ch/event/1073146/contributions/4533631)]
* detailed time-simulation [[time-gate implementation](https://indico.cern.ch/event/937277/contributions/3937623)]
* signal-induced noise (SIN) [[SIN model implementation](https://indico.cern.ch/event/944707/contributions/3969728)][[SIN paper](https://cds.cern.ch/record/2783500)]
* time-gate studies (~SIN, spillover) [[Claro timing properties](https://indico.cern.ch/event/1009732/contributions/4237189)][[edge/level-detection scenarios](https://indico.cern.ch/event/1073146/contributions/4533631)]

**RICH digitisation - including the components mentioned above - is configurable and can be used for various detector studies. See description [here](https://gitlab.cern.ch/lhcb/Boole/-/blob/master/Rich/RichDigiSys/python/RichDigiSys/Configuration.py) for the meaning of the various parameters and their configuration options. Example options for this kind of studies are also available [here](common/options/Boole/).**

The simplest way to start such a study (for example to test different time-gate scenarios) is given below.
```
rich-make-study <yourStudyFolderName>
# (then modify the following lines in the Makefile)
LIST_OF_JOBS=digi # (replace contents of the original variable)
# GENERIC_OPTIONS_BOOLE=$${PWD}/Boole-Job-Generic.py # (uncomment this line and comment out the original one)
# (and continue with)
make jobs
# (copy example options from `common/options/Boole/Boole-Job-Generic.py` to each job folder - `digi/` in the current example - and modify the options accordingly)
make run
```

## Performance studies
The following components of the RICH performance are evaluated using the simulation:
* Cherenkov photons yield
* resolution of the measured Cherenkov angle (including individual contributions: chromatic error, emission point error, pixel error)
* typical detector occupancy
* PID performance (this requires running the reconstruction using `Moore` over the produced simulation samples)

The first two components are studied using a particle gun with dedicated configuration that allows estimating these contributions in regions of both RICH detectors where optimal performance is expected ([see](https://indico.cern.ch/event/985420/contributions/4149774) details). The estimated performance can be seen in `output/performance` folders of the predefined jobs `pGun_rich1` / `pGun_rich2` for RICH1 / RICH2.

Detector occupancy and PID performance are studied using typical signal events ([see](https://indico.cern.ch/event/985420/contributions/4149774) details). The estimated occupancy can be seen in the `output/occupancy` folder of the predefined `bEvent_basic` job (or simply `bEvent` for higher statistics). Similarly, the expected PID results are shown in the `output/PID` folder.
