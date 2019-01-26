#!/bin/csh
#
#  Each job generates 'num_events'
#  Events will be numbered from 1 to 'num_jobs * num_events'
#  Generation seed is taken from run number and event number
#  Ensure that for a given 'event_code' run numbers and event numbers 
#  are unique in all productions
#  Combination of names 'prod_names' and 'top_dir_sim' should be unique
#  to avoid risk of overriting previously generated data. 
#  It is checked and script return error in such case.
#
set first_event=1
set num_events=50
set run_number=13571
set prod_name=1
set event_code=13104011

set top_dir_sim="$RICH_DATA/mapmt_v1/gauss"

# first job number and how many jobs to submit
set job=1
set num_jobs=400

# check for configuration errors
if ( ! -e ${top_dir_sim} ) then
   echo " "
   echo '==========================================================='
   echo "Top level production directory does not exist: $top_dir_sim"
   echo "Error. Exiting now."
   echo '==========================================================='
   echo " "
   exit 1
endif

if ( ! -w ${top_dir_sim} ) then
   echo " "
   echo '==========================================================='
   echo "Cannot write in top level production directory: $top_dir_sim"
   echo "Error. Exiting now."
   echo '==========================================================='
   echo " "
   exit 1
endif

setenv production_dir ${top_dir_sim}/${prod_name} 
if ( -e $production_dir ) then
   echo " "
   echo '==========================================================='
   echo "Production directory already exists: $production_dir "
   echo "Risk of overwriting previously generated data."
   echo "Please, use unique production name."
   echo "Error. Exiting now."
   echo '==========================================================='
   echo " "
   exit 1
else
   mkdir $production_dir
endif

# create directories to store generated data and logs
mkdir ${production_dir}/data
mkdir ${production_dir}/logs
# create execute dir if TMPDIR (PBS) is not available
mkdir ${production_dir}/work

# 1      11164401     B->D- rho                         
# 2      11164402     B-> D*- pi+, D*- ->  D- pi0       
# 3      11164403     B-> D*- K+, D*- ->  D- pi0        
# 4      11264000     B-> D- pi+                        
# 5      11264010     B-> D- K+                         
# 6      15164001     Lb-> Lc pi                        
# 7      13436000     Bs->chic phi

if (-e $prod_name ) rm -rf $prod_name
mkdir $prod_name
set nev=$num_events

set ijob=0
loopjob:
echo "$ijob $job $first_event"
if ( -e job_${job}.csh ) rm -f job_${job}.csh
sed s/ins_job/"$job"/ run_job.csh | sed s/ins_firstevent/"$first_event"/ | sed s/ins_numevents/"$num_events"/ | sed s/ins_runnumber/"$run_number"/ | sed s/ins_prodname/"$prod_name"/ | sed s/ins_eventcode/"$event_code"/ | sed s]ins_topdirsim]"$top_dir_sim"] > ${prod_name}/job_${job}.csh
cd ${prod_name}
chmod a+x job_${job}.csh

#source job_${job}.csh
bsub -q 1nd -J ${prod_name}_${job} job_${job}.csh
sleep 1
cd ../
#echo "qsub -q l_long -N ${prod_name}_${job} ${prod_name}/job_${job}.csh"
@ ijob += 1
@ job += 1
@ first_event += $nev
if ( $ijob < $num_jobs ) goto loopjob
