#pragma once
#include "types.h"

typedef uint Shader;
typedef uint Uniform;

func Shader shader_load(const char* restrict name);
func Shader shader_uniform(Shader shd, const char* name);
func void shader_unload(Shader shd);
func void shader_bind(Shader shd);
func void shader_unbind(void);

