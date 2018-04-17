#!/bin/bash -l
##$ -l h_rt=0:01:00
#SBATCH --constraint=dwarves
# #SBATCH --nodes=1 --ntasks-per-node=4

module load OpenMPI

mpirun /homes/dan/625_slurm/pt2
