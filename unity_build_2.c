#include "glad.c"

#include "headers/memory.h"
#include "headers/debug.h"

#ifdef TCC
#  define STBI_NO_SIMD
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC mem_alloc
#define STBI_REALLOC mem_realloc
#define STBI_FREE mem_free
#define STBI_ASSERT assert
#include <stb_image.h>

#ifdef TCC
int fopen_s(FILE**,const char*,const char*);
size_t strnlen(const char*,size_t l);
#endif

#include <stb_vorbis.h>
