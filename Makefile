CC = gcc
CFLAGS= -Wall -O3 -g -lm
LIST = scan

all: ${LIST}

util.o: util.c util.h
monitoring.o: util.o monitoring.c monitoring.h 
scan.o: scan.c scan.h

scan: monitoring.o scan.o util.o 
	${CC} -Wall -O3 -g scan.o util.o monitoring.o -o scan -lm



clean:
	rm -f ${LIST} *.o
