#!/bin/csh
##############################################################################
# FILE: batchscript.c
# DESCRIPTION:
#   Simple Slurm job command script for MPI exercise
# pthreads
# AUTHOR: Blaise Barney
# LAST REVISED: 05/23/17
##############################################################################

##### These lines are for Slurm
#SBATCH -N 1                      #Number of nodes to use
#SBATCH -t 60:00                   #Maximum time required
#SBATCH -o output.%j              #Output file name

#SBATCH --constraint=elves

### Job commands start here
### Display some diagnostic information
echo '=====================JOB DIAGNOTICS========================'
date
echo -n 'This machine is '; hostname
echo -n 'My jobid is '; echo $SLURM_JOBID
echo 'My path is:'
echo $PATH
echo 'My job info:'
squeue -j $SLURM_JOBID
echo 'Machine info'
sinfo -s

echo '=====================JOB STARTING=========================='

/usr/bin/time -v /homes/travis14/3way /homes/travis14/wiki_dump.txt 

echo ' '
echo '========================ALL DONE==========================='

