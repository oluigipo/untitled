// Includes
#define NDEBUG
#include "headers/all.h"

struct GameGlobalState game = { 0 };

internal void parse_args(struct GameArgs* restrict args, uint argc, const char* restrict* restrict argv) {
	// Default values
	args->mem = megabytes(8);
	args->width = 0;
	args->height = 0;
	args->fullscreen = -1;
	args->novsync = 0;
	args->locale = -1;
	
	// debug_log("%llu\n", hash_of_cstr("locale"));
	
	// Parse arguments
	for (uint i = 1; i < argc; ++i) {
		const char* restrict arg = argv[i];
		
		// Argument shall begin with a -
		if (arg[0] != '-') {
			continue;
		}
		
		u64 hash = hash_of_cstr(arg + 1); // ignore first character
		
#define __write_field(field, format) \
do { if (!argv[i+1]) { debug_error("Missing value for argument '%s'. Default to %u.\n", argv[i], args->field); break; } ++i; arg = argv[i]; sscanf(arg, (format), &args->field); } while (0)
#define __write_flag(field, value) do { args->field = (value); } while (0)
		
		switch (hash) {
			case 3298519710295134399ull: __write_field(width, "%u"); break;
			case 14734894603901239938ull: __write_field(height, "%u"); break;
			case 2222796463784432279ull: __write_field(locale, "%u"); break;
			case 3748217411440170462ull: // fs
			case 17893791189687017496ull: __write_flag(fullscreen, true); break;
			case 16136394061700334151ull: __write_flag(novsync, true); break;
			
			// mem
			case 15779855643795709214ull: {
				++i;
				arg = argv[i];
				if (!arg) {
					debug_error("Missing value for argument '-mem'. Default to %zu.\n", args->mem);
					
					break;
				}
				
				usize s;
				sscanf(arg, "%zu", &s);
				
				if (s < args->mem) {
					debug_error("Argument for '-mem' shall be greater than %u. Ignoring flag.\n", megabytes(1));
					
					break;
				}
				
				args->mem = s;
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
	// Main things setup
	setvbuf(stdout, NULL, _IONBF, 0);
	
	// Initialize game.
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
