
CC=g++
CFLAGS=-std=c++11
LDFLAGS=-lm -lao -Lfft
FFT_DIR=fft

#http://www.cs.duke.edu/~ola/courses/programming/Makefiles/node7.html


all: test sound-raspi input

main.o: main.cpp morsetext.h
	$(CC) -c $(CFLAGS) $< -o $@

morsetext.o: morsetext.cpp morsetext.h
	$(CC) -c $(CFLAGS) $< -o $@

sound-raspi.o: sound-raspi.cpp morsetext.h sound.h
	$(CC) -c $(CFLAGS) $< -o $@

test: main.o morsetext.o
	$(CC) $(LDFAGS) $^ -o $@

sound-raspi: morsetext.o sound-raspi.o
	$(CC) -o $@ $^ $(LDFLAGS)
	
#INPUT
input.o: input.cpp sound.h
	$(CC) -c $(CFLAGS) $< -o $@
	
input: input.o morsetext.o $(FFT_DIR)/fft.o $(FFT_DIR)/complex.o
	$(CC) $(LDFAGS) $^ -o $@
	
#FFT
fftlib:
	$(MAKE) -C $(FFT_DIR)


clean:
	rm *.o test sound-raspi
