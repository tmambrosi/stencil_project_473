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
#SBATCH -t 00:20:00        

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

# i just added this..
module load gcc openmpi

#Run the job


debug=0
iter=10    # num iterations 

INITIAL=initial.dat  # input data file name
FINAL=final.dat      # final state file name
ALL=all.raw          # stacked images 
GOLDEN=golden.dat

PATH=/scratch/$USER/job_$SLURM_JOB_ID

make clean all

pth_out_file="pth_data"
mpi_out_file="mpi_data"

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

echo "--pth--"
for i in "${n[@]}"
do
    ./make-2d $i $i $PATH/$INITIAL
    ./stencil-2d $iter $PATH/$INITIAL $PATH/$GOLDEN

        for j in "${threads[@]}"
        do
        ./make-2d $i $i $PATH/$INITIAL

        ./pth-stencil-2d $iter $PATH/$INITIAL $PATH/$FINAL $debug $j >> $PATH/${pth_out_file}_${i}.txt

        /usr/bin/cp $PATH/${pth_out_file}_${i}.txt ./

        diff $PATH/$FINAL $PATH/$GOLDEN
        done
done

number=0
echo "--mpi--"
for i in "${n[@]}"
do
    ./make-2d $i $i $PATH/$INITIAL
    ./stencil-2d $iter $PATH/$INITIAL $PATH/$GOLDEN

        for j in "${threads[@]}"
        do
        ./make-2d $i $i $PATH/$INITIAL

        mpirun -np $j ./mpi-stencil-2d $iter $PATH/$INITIAL $PATH/$FINAL 0 >> $PATH/${mpi_out_file}_${i}.txt

        /usr/bin/cp $PATH/${mpi_out_file}_${i}.txt

        diff $PATH/$FINAL $PATH/$GOLDEN
        done
done

echo "--mpi finish--"

python3 graph_gen.py
# copy  data files out of scratch space to current working directory

/usr/bin/cp $PATH/$INITIAL ./
/usr/bin/cp $PATH/$FINAL ./
# /usr/bin/cp $PATH/$ALL ./

# how to tell your usage
# make sure this module is loaded: module load sdsc
# then run this command:
# expanse-client project ccu107 -p | grep $USER
