
CC=g++
CFLAGS=-std=c++11

complex.o: complex.cpp complex.h
	$(CC) -c $(CFLAGS) $< -o $@
	
fft.o: fft.cpp complex.h fft.h
	$(CC) -c $(CFLAGS) $< -o $@
