// Includes
#define NDEBUG
#define __CREATE_IMPL__
#include "headers/std.h"
#include "headers/all.h"

// STB Image
#ifdef TCC
#define STBI_NO_SIMD
#endif
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC mem_alloc
#define STBI_REALLOC mem_realloc
#define STBI_FREE mem_free
#define STBI_ASSERT assert
#include <stb_image.h>

struct GameGlobalState game = { 0 };

internal u64 hash_of(const char* restrict str) {
	u64 hash = 2166136261ull;
	
	while (*str) {
		u8 value = (u64)*str++;
		if (value > 0b01100000)
			value &=~ 0b00100000;
		
		hash ^= value;
		hash *= 16777619ull;
	}
	
	return hash;
}

internal void parse_args(struct GameArgs* restrict args, uint argc, const char* restrict* restrict argv) {
	// Default values
	args->arena = 1024 * 1024;
	args->width = 1280;
	args->height = 720;
	args->fullscreen = false;
	args->novsync = false;
	
	// Parse arguments
	for (uint i = 1; i < argc; ++i) {
		const char* restrict arg = argv[i];
		
		// Argument shall begin with a -
		if (arg[0] != '-') {
			continue;
		}
		
		u64 hash = hash_of(arg + 1); // ignore first character
		
#define __write_field(field, format) \
do { if (!argv[i+1]) { debug_error("Missing value for argument '%s'. Default to %u.\n", argv[i], args->field); break; } ++i; arg = argv[i]; sscanf(arg, (format), &args->field); } while (0)
#define __write_flag(field, value) do { args->field = (value); } while (0)
		
		switch (hash) {
			case 3298519710295134399ull: __write_field(width, "%u"); break;
			case 14734894603901239938ull: __write_field(height, "%u"); break;
			case 3748217411440170462ull: // fs
			case 17893791189687017496ull: __write_flag(fullscreen, true); break;
			case 16136394061700334151ull: __write_flag(novsync, true); break;
			
			// arena
			case 2601567961011631409ull: {
				++i;
				arg = argv[i];
				if (!arg) {
					debug_error("Missing value for argument '-arena'. Default to %zu.\n", args->arena);
					
					break;
				}
				
				usize s;
				sscanf(arg, "%zu", &s);
				
				if (s < args->arena) {
					debug_error("Argument for '-arena' shall be greater than the default value %zu. Ignoring flag.\n", args->arena);
					
					break;
				}
				
				args->arena = s;
			} break;
			
			// error
			case 14697351840100226696ull: {
				++i;
				arg = argv[i];
				if (!arg) {
					debug_error("WTF????");
					exit(1);
				}
				
				os_message_box("Assertion Failure", arg);
				
				exit(0);
			} break;
			
			// Unknown
			default: {
				debug_error("Ignoring argument '%s'\n", arg);
			} break;
		}
		
#undef __write_field
#undef __write_flag
	}
}

//~ Main Function
int main(int argc, char* argv[]) {
	struct GameArgs args;
	parse_args(&args, (uint)argc, (void*)argv);
	
	uint result = engine_init(&args);
	if (result != 0)
		return result;
	
	game.currentScene = scene_main;
	do {
		result = game.currentScene();
	} while (game.currentScene && result == 0);
	
	engine_deinit();
	return result;
}
