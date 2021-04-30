##### docs
# Current workarounds:
# - set DecFiles manually for older DB tags (up to upgrade/dddb-20210218, Gauss/v54r5)

# Keep in mind:
# - configure your study: set software / DB tags / options + #events / queue details + reference job + list to run
# - configure digi jobs: set input dir/files + reference
# - keep an eye on large sim files (overestimates R2 occupancy by ~5%, PID marginally worse)

# Some info on HTCondor:
# - job flavours in condor
#		espresso     = 20 mins
#		microcentury = 1 hour
#		longlunch    = 2 hours
#		workday      = 8 hours
#		tomorrow     = 1 day
#		testmatch    = 3 days
#		nextweek     = 1 week
# - timing
# 		5k EV: no visible stat errors in PID plot (2k ok for checks)
# 		PID  : 2k events - 4h
# 		Gauss: 25 B-events - 15h
# 		Gauss: 10k pGun events - 6h
# 		spillover: 10 EV * 1000 jobs ( tomorrow / 70000 secs )

# Typical job sizes:
# - bEvent (Gauss->Rec) : 10k -> 100GB
# - bEvent for performance (Gauss) : 100k -> 150GB
# - spillover (Gauss[->Brunel]) : 10k -> 50[100]GB
# - digi (Boole->Brunel) : 10k -> 40GB
# - pGun (Gauss) : 25k -> negligible

##### end of docs

###### config

EVT_PER_JOB=20
NUM_JOBS=500
RUN_NUMBER=15375

JOB_FLAVOUR="tomorrow" #queue type in condor
MAX_RUNTIME=86000      #in secs
#JOB_FLAVOUR="workday" #queue type in condor
#MAX_RUNTIME=15000 #in secs
#JOB_FLAVOUR="longlunch" #queue type in condor
#MAX_RUNTIME=7150 #in secs

LIST_OF_JOBS = pGun_rich1 pGun_rich2 bEvent # bEvent{_basic} digi{_noSin} spillover pGun_rich1 pGun_rich2
# add the following for a full performance study with signal: 
# pGun_ref_rich1 pGun_ref_rich2 pGun_ref_goodRegion_rich1 pGun_ref_goodRegion_rich2 bEvent_performance 
LIST_TO_RUN="Gauss Boole Brunel Rec1 Rec2" # Gauss Boole Brunel Rec Rec1 Rec2
LIST_TO_MERGE = Gauss/Gauss-Histo Boole/Boole-Histo Brunel/Brunel-Ntuple Rec1/Rec-Ntuple Rec2/Rec-Ntuple Brunel/Brunel-Histo # Brunel/Brunel-Histo Rec/Rec-Histo Rec1/Rec-Histo Rec2/Rec-Histo Rec/Rec-Ntuple

REFERENCE_DIR=$${RICH_DATA}/Gauss_v54r5/bmalecki-rich-nominalPmtQe/bmalecki-rich-nominalPmtQe/baseline/bEvent
# REFERENCE_DIR=/eos/lhcb/user/b/bmalecki/RICH_Upgrade/Gauss_v54r5/bmalecki-rich-nominalPmtQe/bmalecki-rich-nominalPmtQe/baseline/bEvent

# regular digi jobs (adapt EVT_PER_JOB, NUM_JOBS, queue settings accordingly for special cases)
INPUT_DIR_FOR_DIGI=$${EOS_PREFIX}$(REFERENCE_DIR)/Gauss/data
# INPUT_DIR_FOR_DIGI=$${EOS_PREFIX}/eos/lhcb/user/b/bmalecki/RICH_Upgrade/Gauss_v54r5/bmalecki-rich-nominalPmtQe/bmalecki-rich-nominalPmtQe/baseline/bEvent/Gauss/data
INPUT_FILES_FOR_DIGI='InputArea+\\\\\"/Gauss_$$\{i\}\\\\\"'
# for spillover
# INPUT_DIR_FOR_DIGI=$${EOS_PREFIX}/eos/lhcb/user/b/bmalecki/RICH_Upgrade/Gauss_v54r5/bmalecki-rich-nominalPmtQe/bmalecki-rich-nominalPmtQe/baseline/spillover/Gauss/data
# INPUT_FILES_FOR_DIGI='InputArea+\\\\\"/Gauss_$$\{i\}\\\\\",InputArea+\\\\\"/Gauss_$$\(\($$\{i\}+250\)\)\\\\\",InputArea+\\\\\"/Gauss_$$\(\($$\{i\}+500\)\)\\\\\",InputArea+\\\\\"/Gauss_$$\(\($$\{i\}+750\)\)\\\\\"'
# for full (performance studies)
# INPUT_DIR_FOR_DIGI=$${EOS_PREFIX}/eos/lhcb/user/b/bmalecki/RICH_Upgrade/Gauss_v54r5/bmalecki-rich-nominalPmtQe/bmalecki-rich-nominalPmtQe/baseline/bEvent_performance/Gauss/data
# INPUT_FILES_FOR_DIGI='InputArea+\\\\\"/Gauss_$$\{i\}\\\\\",InputArea+\\\\\"/Gauss_$$\(\($$\{i\}+500\)\)\\\\\",InputArea+\\\\\"/Gauss_$$\(\($$\{i\}+1000\)\)\\\\\",InputArea+\\\\\"/Gauss_$$\(\($$\{i\}+1500\)\)\\\\\",InputArea+\\\\\"/Gauss_$$\(\($$\{i\}+2000\)\)\\\\\"'

DDDB=upgrade/bmalecki-rich-nominalPmtQe		# update QE with T+P corrections
CONDDB=upgrade/bmalecki-rich-nominalPmtQe	# update QE with T+P corrections
# DDDB=upgrade/dddb-20210218				# latest ref
# CONDDB=upgrade/sim-20201218-vc-md100		# latest ref
# DDDB=upgrade/dddb-20201105				# update SIN model
# CONDDB=upgrade/sim-20201218-vc-md100		# update SIN model
# DDDB=upgrade/dddb-20201105				# UT update
# CONDDB=upgrade/sim-20201030-vc-md100		# UT update
# DDDB=upgrade/dddb-20201030 				# add simulation input updates
# CONDDB=upgrade/sim-20201030-vc-md100 		# add simulation input updates
# DDDB=upgrade/dddb-20201029 				# add link maps (Chris) + before simulation input updates (direct)
# CONDDB=upgrade/sim-20201029-vc-md100 		# add link maps (Chris) + before simulation input updates (direct)
# DDDB=upgrade/dddb-20200529				# FT update
# CONDDB=upgrade/sim-20200515-vc-md100		# FT update
# DDDB=upgrade/dddb-20200508				# add channel properties
# CONDDB=upgrade/sim-20200508-vc-md100		# add channel properties
# DDDB=upgrade/dddb-20190912				# UT update + largePmtFlagFix
# CONDDB=upgrade/sim-20190912-vc-md100		# UT update + largePmtFlagFix
# DDDB=upgrade/dddb-20190726				# add new numbering scheme
# CONDDB=upgrade/sim-20190726-vc-md100		# add new numbering scheme
# DDDB=upgrade/dddb-20190223				# before new numbering scheme (direct)
# CONDDB=upgrade/sim-20180530-vc-md100		# before new numbering scheme (direct)

RUN_COMMAND_GAUSS="$${RICH_BASE_SOFTWARE}/stack-gauss/Gauss/build.$${CMTCONFIG_GAUSS}/run -s GITCONDDBPATH=$${RICH_BASE_SOFTWARE}/data"
# RUN_COMMAND_GAUSS="$${RICH_BASE_SOFTWARE}/stack-gauss/Gauss/build.$${CMTCONFIG_GAUSS}/run -s GITCONDDBPATH=$${RICH_BASE_SOFTWARE}/data -s DECFILESOPTS=/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/Gen/DecFiles/v31r4/options -s DECFILESROOT=/cvmfs/lhcb.cern.ch/lib/lhcb/DBASE/Gen/DecFiles/v31r4" # for older DB tags
# RUN_COMMAND_GAUSS="$${RICH_BASE_SOFTWARE}/dedicated/study-performance/gauss-stack/Gauss/build.$${CMTCONFIG_GAUSS}/run -s GITCONDDBPATH=$${RICH_BASE_SOFTWARE}/data"
RUN_COMMAND_BOOLE="$${RICH_BASE_SOFTWARE}/release/Boole_v41r3/build.$${CMTCONFIG_STACK}/run -s GITCONDDBPATH=$${RICH_BASE_SOFTWARE}/data"
# RUN_COMMAND_BOOLE="$${RICH_BASE_SOFTWARE}/stack-lhcb/Boole/build.$${CMTCONFIG_STACK}/run -s GITCONDDBPATH=$${RICH_BASE_SOFTWARE}/data"
RUN_COMMAND_BRUNEL="$${RICH_BASE_SOFTWARE}/stack-lhcb/Brunel/build.$${CMTCONFIG_STACK}/run -s GITCONDDBPATH=$${RICH_BASE_SOFTWARE}/data"
RUN_COMMAND_REC="$${RICH_BASE_SOFTWARE}/stack-lhcb/Rec/build.$${CMTCONFIG_STACK}/run -s GITCONDDBPATH=$${RICH_BASE_SOFTWARE}/data"

GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-Generic.py
# GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-Generic-Spillover.py
# GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-Generic-OldPMT.py
# GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Generic-Rich1.py
# GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Generic-Rich2.py
# GENERIC_OPTIONS_GAUSS=$${PWD}/Gauss-Job-Generic.py
GENERIC_OPTIONS_BOOLE=$${RICH_BASE_OPTIONS}/Boole/Boole-Job-Generic.py
# GENERIC_OPTIONS_BOOLE=$${RICH_BASE_OPTIONS}/Boole/Boole-Job-Generic-NoSin.py
# GENERIC_OPTIONS_BOOLE=$${PWD}/Boole-Job-Generic.py
GENERIC_OPTIONS_BRUNEL=$${RICH_BASE_OPTIONS}/Brunel/Brunel-Job-Generic.py
# GENERIC_OPTIONS_BRUNEL=$${RICH_BASE_OPTIONS}/Brunel/Brunel-Job-Generic-NoData.py
# GENERIC_OPTIONS_BRUNEL=$${PWD}/Brunel-Job-Generic.py
GENERIC_OPTIONS_REC=$${RICH_BASE_SOFTWARE}/stack-lhcb/Rec/Rich/RichFutureRecSys/examples/RichFuture.py

##### end of config

# useful variables
CURRENT_DIR=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
CURRENT_DIR_RELATIVE=`echo $(CURRENT_DIR) | sed -e 's/^.*jobs\///'`
CURRENT_OUTPUT_DIR=$(RICH_DATA)/$(CURRENT_DIR_RELATIVE)

# rules

# default
.PHONY: all
all: plots

.PHONY: config
config:
	@echo -e "Study directory:\n\t$(CURRENT_DIR)"
	@echo -e "Study data:\n\t$(CURRENT_OUTPUT_DIR)"
	@echo -e "Reference directory:\n\t$(REFERENCE_DIR)"
	@echo -e "List of jobs:\n\t$(LIST_OF_JOBS)"
	@echo -e "Apps to run:\n\t$(LIST_TO_RUN)"

.PHONY: jobs
jobs:
	# check if RICH environment is set
	@if [ -z "$$RICH_HOST" ]; then \
		echo -e "\nRICH environment not set. Exitting.\n"; \
		exit 1; \
	fi
	# prepare / update jobs
	@for ITEM in test $(LIST_OF_JOBS); do \
		if [ ! -d $${ITEM} ]; then \
			echo -e "Creating new job:\t$${ITEM}"; \
			\
			mkdir -p $$ITEM; \
			cp $(RICH_BASE_SCRIPTS)/run_Job.sh $$ITEM; \
			sed -i 's|^LIST_TO_RUN=.*$$|LIST_TO_RUN=$(LIST_TO_RUN)|' $$ITEM/run_Job.sh; \
			sed -i 's|^DDDB=.*$$|DDDB=$(DDDB)|' $$ITEM/run_Job.sh; \
			sed -i 's|^CONDDB=.*$$|CONDDB=$(CONDDB)|' $$ITEM/run_Job.sh; \
			sed -i 's|^RUN_COMMAND_GAUSS=.*$$|RUN_COMMAND_GAUSS=$(RUN_COMMAND_GAUSS)|' $$ITEM/run_Job.sh; \
			sed -i 's|^RUN_COMMAND_BOOLE=.*$$|RUN_COMMAND_BOOLE=$(RUN_COMMAND_BOOLE)|' $$ITEM/run_Job.sh; \
			sed -i 's|^RUN_COMMAND_BRUNEL=.*$$|RUN_COMMAND_BRUNEL=$(RUN_COMMAND_BRUNEL)|' $$ITEM/run_Job.sh; \
			sed -i 's|^RUN_COMMAND_REC=.*$$|RUN_COMMAND_REC=$(RUN_COMMAND_REC)|' $$ITEM/run_Job.sh; \
			sed -i 's|^GENERIC_OPTIONS_GAUSS=.*$$|GENERIC_OPTIONS_GAUSS=$(GENERIC_OPTIONS_GAUSS)|' $$ITEM/run_Job.sh; \
			sed -i 's|^GENERIC_OPTIONS_BOOLE=.*$$|GENERIC_OPTIONS_BOOLE=$(GENERIC_OPTIONS_BOOLE)|' $$ITEM/run_Job.sh; \
			sed -i 's|^GENERIC_OPTIONS_BRUNEL=.*$$|GENERIC_OPTIONS_BRUNEL=$(GENERIC_OPTIONS_BRUNEL)|' $$ITEM/run_Job.sh; \
			sed -i 's|^GENERIC_OPTIONS_REC=.*$$|GENERIC_OPTIONS_REC=$(GENERIC_OPTIONS_REC)|' $$ITEM/run_Job.sh; \
			sed -i 's|^EVT_PER_JOB=.*$$|EVT_PER_JOB=$(EVT_PER_JOB)|' $$ITEM/run_Job.sh; \
			sed -i 's|^NUM_JOBS=.*$$|NUM_JOBS=$(NUM_JOBS)|' $$ITEM/run_Job.sh; \
			sed -i 's|^RUN_NUMBER=.*$$|RUN_NUMBER=$(RUN_NUMBER)|' $$ITEM/run_Job.sh; \
			sed -i 's|^JOB_FLAVOUR=.*$$|JOB_FLAVOUR=$(JOB_FLAVOUR)|' $$ITEM/run_Job.sh; \
			sed -i 's|^MAX_RUNTIME=.*$$|MAX_RUNTIME=$(MAX_RUNTIME)|' $$ITEM/run_Job.sh; \
			\
			cp $(RICH_BASE_SCRIPTS)/run_Plots.sh $$ITEM; \
			sed -i 's|^REFERENCE_DIR=.*$$|REFERENCE_DIR=$(REFERENCE_DIR)|' $$ITEM/run_Plots.sh; \
			sed -i 's|^IS_PARTICLE_GUN=.*$$|IS_PARTICLE_GUN=false|' $$ITEM/run_Plots.sh; \
			\
			\
			if case $$ITEM in test*) true;; *) false;; esac; then \
				sed -i 's|^EVT_PER_JOB=.*$$|EVT_PER_JOB=1|' $$ITEM/run_Job.sh; \
				sed -i 's|^NUM_JOBS=.*$$|NUM_JOBS=1|' $$ITEM/run_Job.sh; \
				sed -i 's|^GENERIC_OPTIONS_GAUSS=.*|GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Generic-Test.py|' $$ITEM/run_Job.sh; \
				sed -i 's|^LIST_TO_RUN=.*$$|LIST_TO_RUN="Gauss Boole Brunel Rec"|' $$ITEM/run_Job.sh; \
				sed -i 's|^condor_submit|\# condor_submit|' $$ITEM/run_Job.sh; \
				sed -i 's|InputArea = \\".*/\(.*\)/data\\"|InputArea = \\"$${SUBMIT_DIR}/tmp/\1\\"|' $$ITEM/run_Job.sh; \
				sed -i 's|^.*\"cd .*$$TMPDIR.*$$||' $$ITEM/run_Job.sh; \
				sed -i 's|^.*\"sleep $${SLEEP_TIME}\".*$$||' $$ITEM/run_Job.sh; \
				sed -i 's|^.*\"eos cp .*$$||' $$ITEM/run_Job.sh; \
				sed -i 's|^.*\"$${RUN_COMMAND_GAUSS} gaudirun.py.*$$|	echo \"cd $${SUBMIT_DIR}/tmp; rm -rf Gauss/; mkdir Gauss; cd Gauss; $${RUN_COMMAND_GAUSS} gaudirun.py $${OPTIONS_DIR}/Gauss/Gauss-Job_0.py > gauss.log; mv *.sim Gauss_0.sim\" >> start_job.sh|' $$ITEM/run_Job.sh; \
				sed -i 's|^.*\"$${RUN_COMMAND_BOOLE} gaudirun.py.*$$|	echo \"cd $${SUBMIT_DIR}/tmp; rm -rf Boole/; mkdir Boole; cd Boole; $${RUN_COMMAND_BOOLE} gaudirun.py $${OPTIONS_DIR}/Boole/Boole-Job_0.py > boole.log; mv *.digi Boole_0.digi\" >> start_job.sh|' $$ITEM/run_Job.sh; \
				sed -i 's|^.*\"$${RUN_COMMAND_BRUNEL} gaudirun.py.*$$|	echo \"cd $${SUBMIT_DIR}/tmp; rm -rf Brunel/; mkdir Brunel; cd Brunel; $${RUN_COMMAND_BRUNEL} gaudirun.py $${OPTIONS_DIR}/Brunel/Brunel-Job_0.py > brunel.log; mv *.xdst Brunel_0.xdst\" >> start_job.sh|' $$ITEM/run_Job.sh; \
				sed -i 's|^.*\"$${RUN_COMMAND_REC} gaudirun.py.*$$|	echo \"cd $${SUBMIT_DIR}/tmp; rm -rf Rec/; mkdir Rec; cd Rec; $${RUN_COMMAND_REC} gaudirun.py $${OPTIONS_DIR}/Rec/Rec-Job_0.py > rec.log\" >> start_job.sh|' $$ITEM/run_Job.sh; \
				\
				sed -i 's|^INPUT_DIR=.*$$|INPUT_DIR=$(CURRENT_DIR)/test/tmp|' $$ITEM/run_Plots.sh; \
				sed -i 's|^IS_PARTICLE_GUN=.*$$|IS_PARTICLE_GUN=true|' $$ITEM/run_Plots.sh; \
			elif case $$ITEM in pGun*) true;; *) false;; esac; then \
				sed -i 's|^EVT_PER_JOB=.*$$|EVT_PER_JOB=1000|' $$ITEM/run_Job.sh; \
				sed -i 's|^NUM_JOBS=.*$$|NUM_JOBS=25|' $$ITEM/run_Job.sh; \
				sed -i 's|^JOB_FLAVOUR=.*$$|JOB_FLAVOUR="workday"|' $$ITEM/run_Job.sh; \
				sed -i 's|^MAX_RUNTIME=.*$$|MAX_RUNTIME=15000|' $$ITEM/run_Job.sh; \
				sed -i 's|^LIST_TO_RUN=.*$$|LIST_TO_RUN="Gauss"|' $$ITEM/run_Job.sh; \
				\
				sed -i 's|^IS_PARTICLE_GUN=.*$$|IS_PARTICLE_GUN=true|' $$ITEM/run_Plots.sh; \
				\
				if case $$ITEM in *rich1*) true;; *) false;; esac; then \
					sed -i 's|^GENERIC_OPTIONS_GAUSS=.*|GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Generic-Rich1.py|' $$ITEM/run_Job.sh; \
					if case $$ITEM in *ref*) true;; *) false;; esac; then \
						sed -i 's|^GENERIC_OPTIONS_GAUSS=.*|GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Generic-Rich1-RefForSignal.py|' $$ITEM/run_Job.sh; \
					fi; \
					if case $$ITEM in *ref_goodRegion*) true;; *) false;; esac; then \
						sed -i 's|^GENERIC_OPTIONS_GAUSS=.*|GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Generic-Rich1-RefForSignal-GoodRegion.py|' $$ITEM/run_Job.sh; \
					fi; \
				elif case $$ITEM in *rich2*) true;; *) false;; esac; then \
					sed -i 's|^GENERIC_OPTIONS_GAUSS=.*$$|GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Generic-Rich2.py|' $$ITEM/run_Job.sh; \
					if case $$ITEM in *ref*) true;; *) false;; esac; then \
						sed -i 's|^GENERIC_OPTIONS_GAUSS=.*|GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Generic-Rich2-RefForSignal.py|' $$ITEM/run_Job.sh; \
					fi; \
					if case $$ITEM in *ref_goodRegion*) true;; *) false;; esac; then \
						sed -i 's|^GENERIC_OPTIONS_GAUSS=.*|GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-ParticleGun-Generic-Rich2-RefForSignal-GoodRegion.py|' $$ITEM/run_Job.sh; \
					fi; \
				fi; \
			elif case $$ITEM in digi*) true;; *) false;; esac; then \
				sed -i 's|^JOB_FLAVOUR=.*$$|JOB_FLAVOUR="longlunch"|' $$ITEM/run_Job.sh; \
				sed -i 's|^MAX_RUNTIME=.*$$|MAX_RUNTIME=7150|' $$ITEM/run_Job.sh; \
				sed -i 's|^EVT_PER_JOB=.*$$|EVT_PER_JOB=40|' $$ITEM/run_Job.sh; \
				sed -i 's|^NUM_JOBS=.*$$|NUM_JOBS=$(NUM_JOBS)|' $$ITEM/run_Job.sh; \
				sed -i 's|^LIST_TO_RUN=.*$$|LIST_TO_RUN="Boole Brunel"|' $$ITEM/run_Job.sh; \
				sed -i 's|InputArea = \\".*/Gauss/data\\"|InputArea = \\"$(INPUT_DIR_FOR_DIGI)\\"|' $$ITEM/run_Job.sh; \
				sed -i 's|myInputFile = \[ InputArea+\\"/Gauss_$${i}\\" \]|myInputFile = \[ $(INPUT_FILES_FOR_DIGI) \]|' $$ITEM/run_Job.sh; \
				sed -i 's|^GENERIC_OPTIONS_BRUNEL=.*|GENERIC_OPTIONS_BRUNEL=$${RICH_BASE_OPTIONS}/Brunel/Brunel-Job-Generic-NoData.py|' $$ITEM/run_Job.sh; \
				if case $$ITEM in digi_noSin*) true;; *) false;; esac; then \
					sed -i 's|^GENERIC_OPTIONS_BOOLE=.*|GENERIC_OPTIONS_BOOLE=$${RICH_BASE_OPTIONS}/Boole/Boole-Job-Generic-NoSin.py|' $$ITEM/run_Job.sh; \
				fi; \
			elif case $$ITEM in spillover*) true;; *) false;; esac; then \
				sed -i 's|^EVT_PER_JOB=.*$$|EVT_PER_JOB=10|' $$ITEM/run_Job.sh; \
				sed -i 's|^NUM_JOBS=.*$$|NUM_JOBS=1000|' $$ITEM/run_Job.sh; \
				sed -i 's|^JOB_FLAVOUR=.*$$|JOB_FLAVOUR="tomorrow"|' $$ITEM/run_Job.sh; \
				sed -i 's|^MAX_RUNTIME=.*$$|MAX_RUNTIME=70000|' $$ITEM/run_Job.sh; \
				sed -i 's|^LIST_TO_RUN=.*$$|LIST_TO_RUN="Gauss Boole Brunel"|' $$ITEM/run_Job.sh; \
				sed -i 's|^GENERIC_OPTIONS_GAUSS=.*|GENERIC_OPTIONS_GAUSS=$${RICH_BASE_OPTIONS}/Gauss/Gauss-Job-Generic-Spillover.py|' $$ITEM/run_Job.sh; \
			elif case $$ITEM in bEvent_performance*) true;; *) false;; esac; then \
				sed -i 's|^EVT_PER_JOB=.*$$|EVT_PER_JOB=40|' $$ITEM/run_Job.sh; \
				sed -i 's|^NUM_JOBS=.*$$|NUM_JOBS=2500|' $$ITEM/run_Job.sh; \
				sed -i 's|^JOB_FLAVOUR=.*$$|JOB_FLAVOUR="tomorrow"|' $$ITEM/run_Job.sh; \
				sed -i 's|^MAX_RUNTIME=.*$$|MAX_RUNTIME=86000|' $$ITEM/run_Job.sh; \
				sed -i 's|^LIST_TO_RUN=.*$$|LIST_TO_RUN="Gauss"|' $$ITEM/run_Job.sh; \
				sed -i 's|^GENERIC_OPTIONS_BRUNEL=.*|GENERIC_OPTIONS_BRUNEL=$${RICH_BASE_OPTIONS}/Brunel/Brunel-Job-Generic-NoData.py|' $$ITEM/run_Job.sh; \
			elif case $$ITEM in bEvent_basic*) true;; *) false;; esac; then \
				sed -i 's|^EVT_PER_JOB=.*$$|EVT_PER_JOB=20|' $$ITEM/run_Job.sh; \
				sed -i 's|^NUM_JOBS=.*$$|NUM_JOBS=500|' $$ITEM/run_Job.sh; \
				sed -i 's|^JOB_FLAVOUR=.*$$|JOB_FLAVOUR="tomorrow"|' $$ITEM/run_Job.sh; \
				sed -i 's|^MAX_RUNTIME=.*$$|MAX_RUNTIME=86000|' $$ITEM/run_Job.sh; \
				sed -i 's|^LIST_TO_RUN=.*$$|LIST_TO_RUN="Gauss Boole Brunel"|' $$ITEM/run_Job.sh; \
				sed -i 's|^GENERIC_OPTIONS_BRUNEL=.*|GENERIC_OPTIONS_BRUNEL=$${RICH_BASE_OPTIONS}/Brunel/Brunel-Job-Generic-NoData.py|' $$ITEM/run_Job.sh; \
			fi; \
		fi; \
	done

.PHONY: run
run:
	@for ITEM in $(LIST_OF_JOBS); do \
		cd $(CURRENT_DIR)/$$ITEM; \
		./run_Job.sh \& ; \
	done

.PHONY: plots
plots:
	@for ITEM in $(LIST_OF_JOBS); do \
		cd $(CURRENT_DIR)/$$ITEM; \
		./run_Plots.sh \& ; \
	done

.PHONY: merge
merge: data-cleanup
	@if [ ! -d $(CURRENT_OUTPUT_DIR) ]; then \
		echo "Data directory does not exist."; \
	else \
		LOG_FILE=$(CURRENT_DIR)/merge.log; \
		if [ -f $$LOG_FILE ]; then rm $$LOG_FILE; touch $$LOG_FILE; fi; \
		for ITEM in $(LIST_OF_JOBS); do \
			TMP_DIR=$(CURRENT_OUTPUT_DIR)/$$ITEM; \
			for TARGET_FILE in $(LIST_TO_MERGE); do \
				TMP_DIR_SINGLE=$$TMP_DIR/`echo $$TARGET_FILE | cut -d "/" -f 1`; \
				TMP_FILE=`echo $$TARGET_FILE | cut -d "/" -f 2`; \
				if [ -d $${TMP_DIR_SINGLE} ]; then \
					if [ -f $${TMP_DIR_SINGLE}/$${TMP_FILE}.root ]; then eos rm $${TMP_DIR_SINGLE}/$${TMP_FILE}.root; fi; \
					lb-run -c $(CMTCONFIG_ROOT) ROOT hadd -n 0 -ff -k $(EOS_PREFIX)$${TMP_DIR_SINGLE}/$${TMP_FILE}.root $${TMP_DIR_SINGLE}/root/$${TMP_FILE}_*.root >> $${LOG_FILE}; \
				fi; \
			done; \
		done; \
	fi

.PHONY: data-cleanup
data-cleanup:
	@if [ ! -d $(CURRENT_OUTPUT_DIR) ]; then \
		echo "Data directory does not exist."; \
	else \
		for ITEM in $(LIST_OF_JOBS); do \
			TMP_DIR=$(CURRENT_OUTPUT_DIR)/$$ITEM; \
			echo "Doing cleanup in the data directory:"; \
			echo "$$TMP_DIR"; \
			echo ""; \
			\
			for SUBDIR in $$TMP_DIR/*/; do \
				find $$SUBDIR/{data,root}/ -name '*.*' -size -50k -delete; \
			done; \
		done; \
	fi

.PHONY: data-show-list
data-show-list:
	@if [ ! -d $(CURRENT_OUTPUT_DIR) ]; then \
		echo "Data directory does not exist."; \
	else \
		for ITEM in $(LIST_OF_JOBS); do \
			for SUBDIR in $(CURRENT_OUTPUT_DIR)/$$ITEM/*/; do \
				echo "Showing data files from the directory:"; \
				echo "$$SUBDIR"; \
				echo ""; \
				ls -lhS $$SUBDIR/data; \
				echo ""; \
				ls -lhS $$SUBDIR/root; \
				echo ""; \
			done; \
		done; \
	fi

.PHONY: data-rm-digi
data-rm-digi:
	@for ITEM in $(LIST_OF_JOBS); do \
		TMP_DIR=$(CURRENT_OUTPUT_DIR)/$${ITEM}; \
		if [ -d $${TMP_DIR} ]; then \
			echo "Removing digi and reco data in the directory:"; \
			echo "$${TMP_DIR}"; \
			echo ""; \
			for SUBDIR in Boole Brunel Rec; do \
				if [ -d $${TMP_DIR}/$${SUBDIR}/data/ ]; then \
					eos rm -r $${TMP_DIR}/$${SUBDIR}/data/; \
				fi; \
			done; \
		fi; \
	done

.PHONY: data-rm-full
data-rm-full:
	@for ITEM in $(LIST_OF_JOBS); do \
		TMP_DIR=$(CURRENT_OUTPUT_DIR)/$${ITEM}; \
		if [ -d $${TMP_DIR} ]; then \
			echo "Removing data files in the directory:"; \
			echo "$${TMP_DIR}"; \
			echo ""; \
			for SUBDIR in `ls -d $${TMP_DIR}/*/`; do \
				if [ -d $${SUBDIR}data/ ]; then \
					eos rm -r $${SUBDIR}data/; \
				fi; \
			done; \
		fi; \
	done

.PHONY: data-rm-root
data-rm-root:
	@for ITEM in $(LIST_OF_JOBS); do \
		TMP_DIR=$(CURRENT_OUTPUT_DIR)/$${ITEM}; \
		if [ -d $${TMP_DIR} ]; then \
			echo "Removing root files in the directory:"; \
			echo "$${TMP_DIR}"; \
			echo ""; \
			for SUBDIR in `ls -d $${TMP_DIR}/*/`; do \
				if [ -d $${SUBDIR}root/ ]; then \
					eos rm -r $${SUBDIR}root/; \
				fi; \
			done; \
		fi; \
	done
	
.PHONY: clean
clean:
	@echo "Cleaning-up output files for study: $(CURRENT_DIR_RELATIVE)"
	@echo ""
	
	@for ITEM in $(LIST_OF_JOBS); do \
		TMP_DIR=$$ITEM/output; \
		if [ -d $(TMP_DIR) ]; then \
			rm -rf $(TMP_DIR); \
		fi; \
		rm -f $$ITEM/plots.log; \
	done

.PHONY: purge
purge:
	@echo "Removing all jobs and data for study: $(CURRENT_DIR_RELATIVE)"
	@echo $(CURRENT_DIR)
	@echo $(CURRENT_OUTPUT_DIR)
	@echo ""
	
	@for ITEM in test $(LIST_OF_JOBS); do \
		rm -rf $$ITEM; \
	done
	
	@rm -f *.log
	
	@if [ -d $(CURRENT_OUTPUT_DIR) ]; then \
		eos rm -r $(CURRENT_OUTPUT_DIR); \
	fi