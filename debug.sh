#!/bin/bash


# debug.sh by Grace McLewee and Tanner Ambrosino

rm *.dat
# make clean all
make

iter=20

./make-2d 6 4 initial.dat

./serial-2d $iter initial.dat serial.dat all_serial.dat

./pth-stencil-2d $iter initial.dat pth.dat 0 4

#mpirun -np 4 valgrind --suppressions=/usr/share/openmpi/openmpi-valgrind.supp ./mpi-stencil 1 initial.dat mpi.dat 0 
mpirun -np 4 ./mpi-stencil-2d $iter initial.dat mpi.dat 0 

diff -y pth.dat mpi.dat
diff -y serial.dat mpi.dat
# cmp -l serial.dat mpi.dat
diff -y serial.dat pth.dat

# diff all_serial.dat all_pth.dat
# diff all_pth.dat all_mpi.dat
# diff all_serial.dat all_mpi.dat

# wc -c < all_serial.dat
# wc -c < all_pth.dat
# wc -c < all_mpi.dat

# echo "serial"
# ./print-2d serial.dat

echo "pthreads"
./print-2d pth.dat

echo "mpi"
./print-2d mpi.dat