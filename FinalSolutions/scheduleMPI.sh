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
#SBATCH --ntask=32

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
#module to load
module load OpenMPI/2.1.1-GCC-7.2.0-2.29 
#Compile an exercise code
mpicc 3way-mpi.c -o mpi 
#Run the code
#time mpirun -np 16 mpi /homes/dan/625/wiki_dump.txt
#time srun --mpi=pmi2 -t 120:00 --mem-per-cpu=4G -n 2 mpi /homes/alecpruth/Proj4/wiki_dump.txt use for 1000000 because unknown char on line 122748
time srun --mpi=pmi2 -t 120:00 --mem-per-cpu=4G -n 16 mpi /homes/dan/625/wiki_dump.txt


### Issue the sleep so we have time to see the job actually running

echo ' ' 
echo '========================ALL DONE==========================='
