CC = mpicc
GCC = gcc
CFLAGS = -Wall -g -Wpedantic -Wstrict-prototypes -std=gnu99
LFLAGS = -lm
PROGS = sender gather_test
# make-graph-hard-coded

all: $(PROGS)

sender: sender.o
	$(CC) -o sender sender.o

sender.o:	sender.c
	$(CC) $(CFLAGS) -c sender.c

gather_test:gather_test.o utilities.o 
	mpicc -o gather_test gather_test.o utilities.o

gather_test.o: gather_test.c utilities.h
	mpicc $(CFLAGS) -c gather_test.c

utilities.o: utilities.c utilities.h
	$(GCC) $(CFLAGS) -c utilities.c


clean:
	rm -f *.o $(PROGS) *.dat *.raw