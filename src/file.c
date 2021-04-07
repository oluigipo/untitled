#include "headers/file.h"
#include "headers/memory.h"
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

usize file_dump(const char* restrict fname, char** restrict out) {
	FILE* file = fopen(fname, "r");
	if (!file)
		return 0;
	
	fseek(file, 0, SEEK_END);
	usize len = ftell(file);
	rewind(file);
	
	char* buffer = mem_alloc(len + 1);
	if (!buffer) {
		fclose(file);
		return 0;
	}
	
	len = fread(buffer, 1, len, file);
	buffer[len] = 0;
	fclose(file);
	
	*out = buffer;
	return len;
}

usize file_read_line(FILE* file, char* buffer, usize limit) {
	usize len = 0;
	char c;
	
	while (len+1 < limit && (c = fgetc(file)) != '\n' && c != EOF) {
		buffer[len++] = c;
	}
	
	buffer[len] = 0;
	
	return len;
}
