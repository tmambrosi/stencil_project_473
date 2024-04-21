CC = gcc
MPICC = mpicc
CFLAGS = -Wall -g -Wpedantic -Wstrict-prototypes -std=gnu99
LFLAGS = -lm -lpthread
PROGS = make-2d print-2d pth-stencil-2d serial-2d mpi-stencil

all: $(PROGS)

make-2d:	make-2d.o utilities.o
	$(CC) -o make-2d make-2d.o utilities.o $(LFLAGS)

print-2d: print-2d.o utilities.o 
	$(CC) -o print-2d print-2d.o utilities.o $(LFLAGS)

pth-stencil-2d:	pth-stencil-2d.o utilities.o timer.o
	$(CC) -o pth-stencil-2d pth-stencil-2d.o utilities.o timer.o  $(LFLAGS)

mpi-stencil: mpi-stencil.o utilities.o timer.o MyMPI.o
	$(MPICC) -o mpi-stencil mpi-stencil.o utilities.o timer.o MyMPI.o

mpi-stencil.o: mpi-stencil.c utilities.h MyMPI.h
	$(MPICC) $(CFLAGS) -c mpi-stencil.c

serial-2d:	serial-2d.o utilities.o timer.o
	$(CC) -o serial-2d serial-2d.o utilities.o timer.o  $(LFLAGS)

make-2d.o:	make-2d.c utilities.h
	$(CC) $(CFLAGS) -c make-2d.c

print-2d.o: print-2d.c utilities.h
	$(CC) $(CFLAGS) -c print-2d.c

pth-stencil-2d.o: pth-stencil-2d.c utilities.h
	$(CC) $(CFLAGS) -c pth-stencil-2d.c

serial-2d.o: serial-2d.c utilities.h
	$(CC) $(CFLAGS) -c serial-2d.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) -c timer.c

MyMPI.o: MyMPI.c MyMPI.h
	$(MPICC) -c MyMPI.c

clean:
	rm -f *.o $(PROGS) *.dat *.raw *.txt