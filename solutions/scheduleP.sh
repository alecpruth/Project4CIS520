#!/bin/csh
##############################################################################
# FILE: batchscript.c
# DESCRIPTION:
#   Simple Slurm job command script for MPI exercise
# AUTHOR: Blaise Barney
# LAST REVISED: 05/23/17
##############################################################################

##### These lines are for Slurm
#SBATCH -N 1                      #Number of nodes to use
#SBATCH -p pReserved              #Workshop queue
#SBATCH -t 15:00                   #Maximum time required
#SBATCH -o output.%j              #Output file name
#SBATCH --mem-per-cpu=1G
#SBATCH --ntask=16

### Job commands start here 
### Display some diagnostic information
echo '=====================JOB DIAGNOTICS========================'
date
echo -n 'This machine is ';hostname
echo -n 'My jobid is '; echo $SLURM_JOBID
echo 'My path is:' echo $PATH
echo 'My job info:'
squeue -j $SLURM_JOBID
echo 'Machine info'
sinfo -s

echo '=====================JOB STARTING=========================='

### CHANGE THE LINES BELOW TO SELECT DIFFERENT MPI CODES AND/OR COMPILERS
#Compile an exercise code
gcc -w -g longsubstr.c -o longsubstr -lpthread
#Run the code
time srun -N1 --mem-per-cpu=200M -t16 ./longsubstr /homes/dan/625/wiki_dump.txt

### Issue the sleep so we have time to see the job actually running
sleep 120
echo ' ' 
echo '========================ALL DONE==========================='
