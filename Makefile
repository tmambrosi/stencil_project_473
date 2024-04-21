CC = gcc
MPICC = mpicc
CFLAGS = -Wall -g -Wpedantic -Wstrict-prototypes -std=gnu99
LFLAGS = -lm -lpthread
PROGS = make-2d print-2d pth-stencil-2d stencil-2d mpi-stencil-2d

all: $(PROGS)

make-2d:	make-2d.o utilities.o
	$(CC) -o make-2d make-2d.o utilities.o $(LFLAGS)

print-2d: print-2d.o utilities.o 
	$(CC) -o print-2d print-2d.o utilities.o $(LFLAGS)

pth-stencil-2d:	pth-stencil-2d.o utilities.o timer.o
	$(CC) -o pth-stencil-2d pth-stencil-2d.o utilities.o timer.o  $(LFLAGS)

mpi-stencil-2d: mpi-stencil-2d.o utilities.o timer.o MyMPI.o
	$(MPICC) -o mpi-stencil-2d mpi-stencil-2d.o utilities.o timer.o MyMPI.o

mpi-stencil-2d.o: mpi-stencil-2d.c utilities.h MyMPI.h
	$(MPICC) $(CFLAGS) -c mpi-stencil-2d.c

stencil-2d:	stencil-2d.o utilities.o timer.o
	$(CC) -o stencil-2d stencil-2d.o utilities.o timer.o  $(LFLAGS)

make-2d.o:	make-2d.c utilities.h
	$(CC) $(CFLAGS) -c make-2d.c

print-2d.o: print-2d.c utilities.h
	$(CC) $(CFLAGS) -c print-2d.c

pth-stencil-2d.o: pth-stencil-2d.c utilities.h
	$(CC) $(CFLAGS) -c pth-stencil-2d.c

stencil-2d.o: stencil-2d.c utilities.h
	$(CC) $(CFLAGS) -c stencil-2d.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) -c timer.c

MyMPI.o: MyMPI.c MyMPI.h
	$(MPICC) -w -c MyMPI.c

clean:
	rm -f *.o $(PROGS) *.dat *.raw *.txt