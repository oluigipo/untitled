#include "headers/sound.h"
#include "headers/memory.h"
#include <AL/al.h>
#include <AL/alc.h>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.h>

internal ALCdevice* soundDevice;
internal ALCcontext* soundContext;

func void sound_init(void) {
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
	
	alcMakeContextCurrent(soundContext);
	
	debug {
		const ALchar* name = alcGetString(soundDevice, ALC_DEVICE_SPECIFIER);
		debug_log("OpenAL's Device Name: %s\n", name);
	};
	
	sound_listener_position((vec3) { 0 });
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
}

func void sound_deinit(void) {
	alcDestroyContext(soundContext);
	alcCloseDevice(soundDevice);
}

func void sound_listener_position(const vec3 pos) {
	alListener3f(AL_POSITION, pos[0], pos[1], pos[2]);
}

//~ Loading Sounds
func uint sound_load_file(const char* fname) {
	int channels, sampleRate;
	i16* data;
	int sampleCount = stb_vorbis_decode_filename(fname, &channels, &sampleRate, &data);
	
	if (sampleCount == -1) {
		debug_log("something went wrong...\n");
		return 0;
	}
	
	ALenum format = AL_NONE;
	if (channels == 1)
		format = AL_FORMAT_MONO16;
	else if (channels == 2)
		format = AL_FORMAT_STEREO16;
	
	isize size = (isize)sampleCount * (isize)channels * sizeof (*data);
	
	uint bufferId;
	alGenBuffers(1, &bufferId);
	alBufferData(bufferId, format, data, size, sampleRate);
	
	ALenum err = alGetError();
    if(err != AL_NO_ERROR) {
        debug_error("OpenAL Error: %s\n", alGetString(err));
        if(bufferId && alIsBuffer(bufferId))
            alDeleteBuffers(1, &bufferId);
        return 0;
    }
	
	mem_free(data);
	
	return bufferId;
}

func void sound_unload(uint buffer) {
	alDeleteBuffers(1, &buffer);
}

//~ Playing Sounds
func void sound_play_source(uint source) {
	alSourcePlay(source);
}

func void sound_play_source_at(uint source, f32 sampleCount) {
	alSourcef(source, AL_SAMPLE_OFFSET, sampleCount);
	alSourcePlay(source);
}

func void sound_stop_source(uint source) {
	alSourceStop(source);
}

func void sound_pause_source(uint source) {
	alSourcePause(source);
}

func void sound_rewind_source(uint source) {
	alSourceRewind(source);
}

//~ Sources
func uint sound_make_source(void) {
	uint source;
	alGenSources(1, &source);
	alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
	alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSourcef(source, AL_GAIN, 0.5f);
	alSourcef(source, AL_PITCH, 1.0f);
	
	return source;
}

func void sound_delete_source(uint source) {
	alDeleteSources(1, &source);
}

func void sound_source_position(uint source, const vec3 pos) {
	alSource3f(source, AL_POSITION, pos[0], pos[1], pos[2]);
}

func void sound_source_params(uint source, f32 gain, f32 pitch) {
	alSourcef(source, AL_GAIN, gain);
	alSourcef(source, AL_PITCH, pitch);
}

func void sound_source_attenuation(uint source, f32 rolloff, f32 refDistance, f32 maxDistance) {
	alSourcef(source, AL_ROLLOFF_FACTOR, rolloff);
	alSourcef(source, AL_REFERENCE_DISTANCE, refDistance);
	alSourcef(source, AL_MAX_DISTANCE, maxDistance);
}

func void sound_source_buffer(uint source, uint buffer) {
	alSourcei(source, AL_BUFFER, buffer);
}

func f32 sound_source_at(uint source) {
	f32 result;
	alGetSourcef(source, AL_SAMPLE_OFFSET, &result);
	return result;
}

//~ Sound Error
func void sound_error(void) {
	ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        debug_error("OpenAL Error: %s\n", alGetString(err));
    }
}

