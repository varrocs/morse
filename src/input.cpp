

#include <iostream>
#include "sound.h"
#include "fft/complex.h"
#include "fft/fft.h"
#include <math.h>
#include <vector>
#include "morsetext.h"

using namespace std;

const int inputFreq = 48000;
const int sampleLength = 1024;
const int avgBlocks = 32;
const int blockLength = sampleLength / avgBlocks;
const double sampleDuration = 1.0 / (double)inputFreq * (double)sampleLength;

bool isOn(complex* sample, int length) {

	double blocks[avgBlocks];
	int blockLen = 2;

	// Calculate blocks
	for (int i = 0; i < avgBlocks; ++i) {
		double sum = 0.0;
		// Summarize inside the block
		for (int j=0; j < blockLen; ++j) {
			sum += sqrt(sample[ i*blockLen + j ].norm());
		}
		sum /= blockLen;
		blocks[i] = sum;
	}

	int maxBlock;
	double maxValue = 0.0;
	for (int i = 1; i < avgBlocks / 2; ++i) {
		if (maxValue < blocks[i]) {
			maxBlock = i;
			maxValue = blocks[i];
		}
	}

//	cout << "MaxBlock: " << maxBlock << " - " << maxValue << endl;

	return maxBlock >= 3 && maxValue > 400.0;
}


int getDurationUnit(double duration) {
	double result = duration / signalUnitDuration;
	//cout << duration << " -> " << result << " -> " << (int)round(result) << endl;
	return (int)round(result);
}

int main(int argc, char**argv) {

	unsigned char min  = 255;
	unsigned char max  = 0;

	// Skip the header
	cin.ignore(50);

	bool lastState = false;
	bool lastSignal = false;
	double lastStateDuration = 0;
	int idleCounter = 0;

	int counter = 0;
	complex sample[sampleLength];
	vector<bool> singalVector;

	while (true) {
		unsigned char next = cin.get();

		sample[counter] = next;
		counter++;

		if (counter >= sampleLength) {
			counter = 0;

			bool ok = CFFT::Forward(sample, sampleLength);
			if (!ok) {
				cerr << "Problem with FFT" << endl;
				exit(1);
			}

#if 0
			// Print blocks
			for (int i = 0; i < avgBlocks / 2; ++i) {
				int sum = 0;
				// Summarize inside the block
				for (int j=0; j < 1/*blockLength*/; ++j) {
					sum += sqrt(sample[ i*blockLength + j ].norm());
					//sum += sample[ i*blockLength + j ].re();
				}
				sum /= /*blockLength*/1;
				cout << sum << "\t";
			}
			cout << endl;
#endif

			bool currentState = isOn(sample, sampleLength);

			lastStateDuration += sampleDuration;

			if (currentState xor lastState) {
//				cout << "State change from " << lastState << " last state duration: " << lastStateDuration<< endl;
				int morseUnit = getDurationUnit(lastStateDuration);

				//cout << lastState << "-" <<morseUnit<<endl;
				if (morseUnit == 0) { // Just noise
					continue;
				}

				for (int i = 0; i < morseUnit; ++i) {
					singalVector.push_back(lastState);
				}
				MorseText text(singalVector);
				cout << text.toString() << endl;
				cout << "-------------------------------------------------------------------" << endl;


				lastStateDuration = 0;
				lastState = currentState;
			}


		}


	}


}
