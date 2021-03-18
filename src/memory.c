#include "headers/memory.h"
#include <stdlib.h>
#include <memory.h>
#include "headers/debug.h"

#define MEM_TRACK_HEAP_ALLOCATIONS

void* mem_alloc(usize size) {
	void* ptr = malloc(size);
#ifdef MEM_TRACK_HEAP_ALLOCATIONS
	printf("Allocation Tracking: mem_alloc(%zu) -> %p\n", size, ptr);
#endif
	return ptr;
}

void* mem_alloc_zero(usize count, usize size) {
	void* ptr = calloc(count, size);
#ifdef MEM_TRACK_HEAP_ALLOCATIONS
	printf("Allocation Tracking: mem_alloc_zero(%zu, %zu) -> %p\n", count, size, ptr);
#endif
	return ptr;
}

void* mem_realloc(void* p, usize size) {
	void* ptr = realloc(p, size);
#ifdef MEM_TRACK_HEAP_ALLOCATIONS
	printf("Allocation Tracking: mem_realloc(%p, %zu) -> %p\n", p, size, ptr);
#endif
	return ptr;
}

void* mem_realloc_zero(void* p, usize count, usize size) {
	assert(!"mem_realloc_zero() doesn't have an implementation.");
	void* ptr = NULL;
	
#ifdef MEM_TRACK_HEAP_ALLOCATIONS
	printf("Allocation Tracking: mem_realloc_zero(%p, %zu, %zu) -> %p\n", p, count, size, ptr);
#endif
	return ptr;
}

void mem_free(void* p) {
#ifdef MEM_TRACK_HEAP_ALLOCATIONS
	printf("Allocation Tracking: mem_free(%p)\n", p);
#endif
	
	free(p);
}

void arena_init(Arena* restrict arena, usize size) {
	arena->head = 0;
	arena->size = size;
	arena->buffer = mem_alloc(size);
}

void arena_deinit(Arena* restrict arena) {
	mem_free(arena->buffer);
	arena->head = arena->size = 0;
	arena->buffer = NULL;
}

void* arena_alloc(Arena* restrict arena, usize size) {
	size = (size + 7) & ~7; // 8 byte alignment
	
	if (arena->head + size > arena->size) {
		return NULL;
	}
	
	void* p = arena->buffer + arena->head;
	arena->head += size;
	return p;
}

void* arena_alloc_zero(Arena* restrict arena, usize size) {
	size = (size + 7) & ~7; // 8 byte alignment
	
	if (arena->head + size > arena->size) {
		return NULL;
	}
	
	void* p = arena->buffer + arena->head;
	arena->head += size;
	memset(p, 0, size);
	return p;
}

void arena_clear(Arena* restrict arena) {
	arena->head = 0;
}

