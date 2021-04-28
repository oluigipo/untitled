#include "headers/options.h"
#include "headers/global.h"

internal const char* optionsFilePath = "./options.ini";

func void options_load(void) {
	// Use default values if some fields aren't present.
	options_reset();
	
	//debug_log("%llu\n", hash_of_cstr("generallocale"));
	
	// Loading...
	FILE* file = fopen(optionsFilePath, "r");
	
	if (!file) {
		options_save(); // Just use the already loaded values.
		return;
	}
	
	// Parse file.
	unsigned char buffer[512];
	unsigned char keyname[64] = { 0 };
	usize sectorLength = 0;
	usize len;
	usize head;
	uint line = 1;
	
	while (true) {
		if (feof(file)) {
			break;
		}
		
		head = 0;
		len = file_read_line(file, buffer, sizeof buffer);
		
		// Ignore comments and empty spaces at the beginning and end of lines.
		for (usize i = 0, topRelevant = 0; i < len; ++i) {
			switch (buffer[i]) {
				case ' ': case '\t': if (i == head) ++head; break;
				case ';': len = topRelevant; break;
				default: topRelevant = i; break;
			}
		}
		
		// Ignore empty lines
		if (len == 0)
			goto __beginning;
		
		// NOTE(luigi): need this to use sscanf
		buffer[len] = 0;
		
		if (buffer[head] == '[') {
			// It's a sector!
			usize start = ++head;
			for (; head < len && buffer[head] != ']'; ++head);;
			usize end = head;
			
			sectorLength = end - start;
			memcpy(keyname, buffer + start, sectorLength);
		} else {
			// It's a key-value pair!
			string key = {
				.ptr = buffer + head,
			};
			
			while (buffer[head++] != '=') {
				if (head >= len) {
					debug_log("Expected '=' in configuration file at line %u.\n", line);
					goto __beginning; // ignore line
				}
			}
			
			if (head >= len) {
				debug_log("Expected value after '=' in configuration file at line %u.\n", line);
				goto __beginning;
			}
			
			// Hash key
			key.len = (buffer + head - 1) - key.ptr;
			
			memcpy(keyname + sectorLength, key.ptr, key.len);
			keyname[sectorLength + key.len] = 0;
			u64 keyHash = hash_of_cstr(keyname);
			
			//debug_log("%s\n", keyname);
			
			// Match key
			switch (keyHash) {
				// [video]
				case 10905049475048401944ull: sscanf(buffer + head, "%u", &game.window.width); break;
				case 11748157869929321231ull: sscanf(buffer + head, "%u", &game.window.height); break;
				case 16059922302271978957ull: sscanf(buffer + head, "%u", &game.vsyncEnabled); break;
				//case 9298201933543872523ull: sscanf(buffer + head, "%u", &game.renderer); break;
				case 3456656266570316237ull: sscanf(buffer + head, "%u", &currentLocale); break;
				
				// NOTE(luigi): no need for warning in unknown field (maybe?)
				default: break;
			}
		}
		
		// beginning at the end
		__beginning:; ++line;
	}
	
	fclose(file);
}

func void options_save(void) {
	FILE* file = fopen(optionsFilePath, "w");
	
	if (!file) {
		debug_error("Could not open options file for saving.\n");
		return;
	}
	
	// Write file.
	fprintf(file, "[general]\n");
	fprintf(file, "locale=%u\n", currentLocale);
	fprintf(file, "[video]\n");
	fprintf(file, "width=%u\n", game.window.width);
	fprintf(file, "height=%u\n", game.window.height);
	fprintf(file, "vsync=%u\n", game.vsyncEnabled);
	//fprintf(file, "renderer=%u\n", game.renderer);
}

func void options_reset(void) {
	game.window.width = 1280;
	game.window.height = 720;
	game.vsyncEnabled = true;
	currentLocale = 0;
	//game.renderer = 0;
}

