#pragma once
#include "types.h"

func void sound_init(void);
func void sound_deinit(void);
func void sound_listener_position(const vec3 pos);

// Loading Sounds
func uint sound_load_file(const char* fname);
func void sound_unload(uint buffer);

// Playing Sounds
func void sound_play_source(uint source);
func void sound_play_source_at(uint source, f32 sampleCount);
func void sound_stop_source(uint source);
func void sound_pause_source(uint source);
func void sound_rewind_source(uint source);

// Sources
func uint sound_make_source(void);
func void sound_delete_source(uint source);
func void sound_source_position(uint source, const vec3 pos);
func void sound_source_params(uint source, f32 gain, f32 pitch);
func void sound_source_attenuation(uint source, f32 rolloff, f32 refDistance, f32 maxDistance);
func void sound_source_buffer(uint source, uint buffer);
func f32 sound_source_at(uint source);

// Error
func void sound_error(void);

