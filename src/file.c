#include "headers/file.h"
#include <stdio.h>

usize file_read(const char* restrict fname, char* buffer, usize buffSize) {
	FILE* file = fopen(fname, "r");
	if (!file)
		return 0;
	
	usize len = fread(buffer, 1, buffSize-1, file);
	buffer[len] = 0;
	
	fclose(file);
	return len;
}

