/*
 * sound-raspi.cpp
 *
 *  Created on: 2013.03.12.
 *      Author: varrocs
 */

#include <ao/ao.h>

#include <cstdio>
#include <cmath>
#include <cstring>
#include <vector>
#include "morsetext.h"
#include "sound.h"

#define PRINT_DEBUG

const int format_bits = 16;
const int format_channels = 2;
const int format_rate = 44100;

bool openDevice(ao_device** device) {
	ao_sample_format format;
	int default_driver;
	int buf_size;
	int sample;
	int i;

	/* -- Initialize -- */
	ao_initialize();

	/* -- Setup for default driver -- */
	default_driver = ao_default_driver_id();

	/* -- Setup format -- */
	std::memset(&format, 0, sizeof(format));
	format.bits = format_bits;
	format.channels = format_channels;
	format.rate = format_rate;
	format.byte_format = AO_FMT_LITTLE;

	/* -- Open driver -- */
	*device = ao_open_live(default_driver, &format, NULL /* no options */);
	if (*device == NULL) {
		fprintf(stderr, "Error opening device.\n");
		return false;
	}

	return true;
}

int main(int argc, char** argv) {

	if (argc < 2) {
		printf("Usage:\n%s \"<text to encode>\"\n", argv[0]);
		exit(1);
	}

	// Get the text
	MorseText text(argv[1]);
	std::vector<bool> signalVector;
	text.toSignals(signalVector);

	/* -- Open the sound device -- */
	ao_device* device;
	openDevice(&device);
	int buf_size = format_bits/8 * format_channels * format_rate;
	char *buffer = (char*) calloc(buf_size, sizeof(char));

	bool inSound;

	// 1: pause, short, 3: LOng
	for (auto sig : signalVector) {
#ifdef PRINT_DEBUG
		fprintf(stderr, "%s", sig ? "-":"_");
#endif
		int length = (int)(signalUnitDuration * (double)format_rate );

		/* -- Assemble the data -- */
		for (int i = 0; i < length; i++) {
			int sample;
			if (sig) {
				sample = (int)(0.75 * 32768.0 * sin(2 * M_PI * freq * ((double) i/format_rate)));
			} else {
				sample = 0;
			}

			/* Put the same stuff in left and right channel */
			buffer[4*i] = buffer[4*i+2] = sample & 0xff;
			buffer[4*i+1] = buffer[4*i+3] = (sample >> 8) & 0xff;
		}

		ao_play(device, buffer, format_bits/8 * format_channels * length);
	}

#ifdef PRINT_DEBUG
		fprintf(stderr, "\n");
#endif

// Close down
	ao_close(device);
	ao_shutdown();
}



