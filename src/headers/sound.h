#pragma once
#include "types.h"

void sound_init(void);
void sound_deinit(void);
void sound_listener_position(const vec3 pos);

// Loading Sounds
uint sound_load_file(const char* fname);
void sound_unload(uint buffer);

// Playing Sounds
void sound_play_source(uint source);
void sound_play_source_at(uint source, f32 sampleCount);
void sound_stop_source(uint source);
void sound_pause_source(uint source);
void sound_rewind_source(uint source);

// Sources
uint sound_make_source(void);
void sound_delete_source(uint source);
void sound_source_position(uint source, const vec3 pos);
void sound_source_params(uint source, f32 gain, f32 pitch);
void sound_source_attenuation(uint source, f32 rolloff, f32 refDistance, f32 maxDistance);
void sound_source_buffer(uint source, uint buffer);
f32 sound_source_at(uint source);

// Error
void sound_error(void);

