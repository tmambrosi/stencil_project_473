#!/bin/bash

rows=5000
cols=5000
iter=100
debug=0

out_file="pth_data"

declare -a threads=(
[0]=1
[1]=2
[2]=4
[3]=8
[4]=16
)
declare -a n=(
[0]=20
)

make clean all
        

for i in "${n[@]}"
do
    ./make-2d $i $i initial.dat
    ./pth-stencil-2d $iter initial.dat golden.dat $debug 1

        for j in "${threads[@]}"
        do
        ./make-2d $i $i initial.dat

        ./pth-stencil-2d $iter initial.dat final.dat $debug $j >> ${out_file}_${n}.txt

        diff final.dat golden.dat
        done
done

