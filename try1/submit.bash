#!/bin/bash
#SBATCH --job-name="stencil-2d"
#SBATCH --output="stencil.%j.%N.out"
#SBATCH --partition=shared
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=32GB
#SBATCH --account=ccu107
#SBATCH --export=ALL
#SBATCH -t 00:05:00        

# note about time above, don't use more than 5 minutes for a run
# set problem size and/or iterations such that on 1 thread, takes < 5 minutes

# think about the size of memory, add some padding and set 

# --cpus-per-task=, should be set to the number of threads you may want

# DO NOT USE THE COMPUTE PARTITION

module purge 
# set whatever modules you need 

module load cpu
module load slurm
module load gcc

#Run the job

NI=10    # num iterations 

INITIAL=initial.dat  # input data file name
FINAL=final.dat      # final state file name
ALL=all.raw          # stacked images 

PATH=/scratch/$USER/job_$SLURM_JOB_ID

make clean all

declare -a threads=(
[0]=1
[1]=2
[2]=4
[3]=8
[4]=16
)
declare -a n=(
[0]=5000
[1]=10000
[2]=20000
[3]=40000
)
# lscpu > lscpu.%j.%N.out
echo 'About to run program -- GMM'
for i in "${n[@]}"
do
    echo 'make input matrix'
    ./make-2d $i $i $PATH/$INITIAL
    echo ' '
    for j in "${threads[@]}"
    do
    echo 'run stencil-2d'
    ./stencil-2d $NI $PATH/$INITIAL $PATH/$FINAL 0 $j 
    echo ' '
    done
done

# copy  data files out of scratch space to current working directory

/usr/bin/cp $PATH/$INITIAL ./
/usr/bin/cp $PATH/$FINAL ./
# /usr/bin/cp $PATH/$ALL ./

# how to tell your usage
# make sure this module is loaded: module load sdsc
# then run this command:
# expanse-client project ccu107 -p | grep $USER
