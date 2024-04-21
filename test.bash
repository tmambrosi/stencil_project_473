#!/bin/bash

# rows=5000
# cols=5000
iter=100
debug=0

rm *.txt

pth_out_file="pth_data"
mpi_out_file="mpi_data"

declare -a threads=(
[0]=1
[1]=2
[2]=4
# [3]=8
# [4]=16
)
declare -a n=(
[0]=50
[1]=100
[2]=200
# [3]=4000
)

number=0

make
        
echo "--pth--"
for i in "${n[@]}"
do
    ./make-2d $i $i initial.dat
    ./stencil-2d $iter initial.dat golden.dat all.dat

        for j in "${threads[@]}"
        do
        ./make-2d $i $i initial.dat

        ./pth-stencil-2d $iter initial.dat final.dat $debug $j >> ${pth_out_file}_${i}.txt

        diff final.dat golden.dat
        done
done

number=0
echo "--mpi--"
for i in "${n[@]}"
do
    ./make-2d $i $i initial.dat
    ./stencil-2d $iter initial.dat golden.dat all.dat

        for j in "${threads[@]}"
        do
        ./make-2d $i $i initial.dat

        mpirun -np $j ./mpi-stencil-2d $iter initial.dat final.dat 0 >> ${mpi_out_file}_${i}.txt

        diff final.dat golden.dat
        done
done

python3 graph_gen.py