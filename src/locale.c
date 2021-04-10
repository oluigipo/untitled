#include "headers/locale.h"
#include "headers/memory.h"
#if defined(TCC) || defined(MSVC)
#include <dirent_imp.h>
#else
#include <dirent.h>
#endif

string** locales;
uint localeCount;
uint currentLocale;

internal strbuf* localesBuffer;

internal char* str_jump_to(char* str, char ch) {
	char* r = strchr(str, ch);
	if (r)
		return r;
	return strchr(str, '\0');
}

void locale_init(void) {
	DIR* dir = opendir("locale");
	
	if (!dir) {
		debug_error("Folder 'locale' not found!\n");
		return;
	}
	
	// Identify files
	struct dirent* entry;
	
	List(FILE*) localesFiles;
	list_init(&localesFiles, 3);
	usize desiredSize = 0;
	
	while (entry = readdir(dir), entry) {
		if (entry->d_name[0] == '.') // ignore if name begins with a dot
			continue;
		
		// Make english as default.
		char fullpath[512];
		snprintf(fullpath, sizeof fullpath, "./locale/%s", entry->d_name);
		
		FILE* file = fopen(fullpath, "r");
		if (!file)
			continue;
		
		fseek(file, 0, SEEK_END);
		desiredSize += ftell(file) + 1;
		rewind(file);
		
		list_push(&localesFiles, file);
	}
	
	closedir(dir);
	
	// Read files
	List(string*) localesList;
	list_init(&localesList, 3);
	
	localesBuffer = strbuf_make(desiredSize);
	
	for (usize i = 0; i < localesFiles.len; ++i) {
		char* beginning = localesBuffer->data + localesBuffer->len;
		usize remaining = localesBuffer->cap - localesBuffer->len;
		
		usize read = fread(beginning, 1, remaining, localesFiles.ptr[i]);
		beginning[read] = 0;
		localesBuffer->len += read;
		
		assert(feof(localesFiles.ptr[i]));
		fclose(localesFiles.ptr[i]);
		
		// Read each string in the file
		string* strings = mem_alloc(sizeof(*strings) * TXT__COUNT);
		list_push(&localesList, strings);
		
		strings[0].ptr = beginning;
		usize j = 1;
		
		while (beginning = str_jump_to(beginning, '\n'), *beginning != 0) {
			strings[j-1].len = beginning - strings[j-1].ptr;
			strings[j].ptr = ++beginning;
			++j;
			
			if (j > TXT__COUNT) {
				debug_error("Locale file has more strings than it should have. Ignoring extra strings...\n");
				break;
			}
		}
		
		strings[j-1].len = beginning - strings[j-1].ptr;
	}
	
	// Clean-up
	list_free(&localesFiles);
	locales = localesList.ptr;
	localeCount = localesList.len;
}

void locale_deinit(void) {
	strbuf_free(localesBuffer);
	
	for (uint i = 0; i < localeCount; ++i)
		mem_free(locales[i]);
	
	mem_free(locales);
	locales = NULL;
	localeCount = 0;
	localesBuffer = NULL;
}

void locale_reload(void) {
	locale_deinit();
	locale_init();
}

