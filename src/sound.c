#include "headers/sound.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <sndfile.h>

internal ALCdevice* soundDevice;
internal ALCcontext* soundContext;

internal void sound_load_and_play_file(const char* name, int filetype) {
	SF_INFO sfinfo = { 0 };
	
	SNDFILE* file = sf_open(name, SFM_READ, &sfinfo);
	if (!file) {
		printf("Error : could not open file : %s\n", infilename);
		puts(sf_strerror(NULL));
		return;
	}
	
	
	
	sf_close(infile);
}

void sound_init(void) {
	soundDevice = alcOpenDevice(NULL);
	if (!soundDevice) {
		debug_error("Could not open an OpenAL Device. Program will continue without sound.\n");
		return;
	}
	
	soundContext = alcCreateContext(soundDevice, NULL);
	if (!soundContext) {
		debug_error("Could not create an OpenAL Context. Program will continue without sound.\n");
		return;
	}
	
	const ALchar* name = alcGetString(soundDevice, ALC_DEVICE_SPECIFIER);
	debug_log("OpenAL's Device Name: %s\n", name);
	
	
	sound_load_and_play_file("assets/test.wav");
	
}

void sound_deinit(void) {
	alcDestroyContext(soundContext);
	alcCloseDevice(soundDevice);
}

