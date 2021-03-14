#pragma once

#include "types.h"

typedef uint Shader;
typedef uint Uniform;

Shader shader_load(const char* restrict name);
Shader shader_uniform(Shader shd, const char* name);
void shader_unload(Shader shd);
void shader_bind(Shader shd);
void shader_unbind(void);

