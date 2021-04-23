#include "headers/memory.h"
#include <stdlib.h>
#include <memory.h>
#include "headers/debug.h"
#include "headers/os.h"

//#define MEM_TRACK_HEAP_ALLOCATIONS
#define ALIGNMENT (16-1)

#define ALIGNUP(s) (((s)+ALIGNMENT) & ~ALIGNMENT)

void* mem_alloc(usize size) {
	void* ptr = malloc(size);
	
	if (unlikely(!ptr)) {
		os_message_box("Not enough memory!", "A memory allocation failed to be done. The program will terminate.");
		exit(1);
	}
	
#ifdef MEM_TRACK_HEAP_ALLOCATIONS
	debug_log("Allocation Tracking: mem_alloc(%zu) -> %p\n", size, ptr);
#endif
	return ptr;
}

void* mem_alloc_zero(usize count, usize size) {
	void* ptr = calloc(count, size);
	
	if (unlikely(!ptr)) {
		os_message_box("Not enough memory!", "A memory allocation failed to be done. The program will terminate.");
		exit(1);
	}
	
#ifdef MEM_TRACK_HEAP_ALLOCATIONS
	debug_log("Allocation Tracking: mem_alloc_zero(%zu, %zu) -> %p\n", count, size, ptr);
#endif
	return ptr;
}

void* mem_realloc(void* p, usize size) {
	void* ptr = realloc(p, size);
	
	if (unlikely(!ptr)) {
		os_message_box("Not enough memory!", "A memory allocation failed to be done. The program will terminate.");
		exit(1);
	}
	
#ifdef MEM_TRACK_HEAP_ALLOCATIONS
	printf("Allocation Tracking: mem_realloc(%p, %zu) -> %p\n", p, size, ptr);
#endif
	return ptr;
}

void* mem_realloc_zero(void* p, usize count, usize size) {
	assert(!"mem_realloc_zero() doesn't have an implementation.");
	void* ptr = NULL;
	
	if (unlikely(!ptr)) {
		os_message_box("Not enough memory!", "A memory allocation failed to be done. The program will terminate.");
		exit(1);
	}
	
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

//~ Arena Allocator
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
	size = ALIGNUP(size);
	
	if (arena->head + size > arena->size) {
		assert(!"Arena is full!");
		return NULL;
	}
	
	void* p = (char*)arena->buffer + arena->head;
	arena->head += size;
	return p;
}

void* arena_alloc_zero(Arena* restrict arena, usize size) {
	size = ALIGNUP(size);
	
	if (arena->head + size > arena->size) {
		assert(!"Arena is full!");
		return NULL;
	}
	
	void* p = (char*)arena->buffer + arena->head;
	arena->head += size;
	memset(p, 0, size);
	return p;
}

void arena_clear(Arena* restrict arena) {
	arena->head = 0;
}

//~ Stack Allocator
void stack_init(Stack* stack, usize size) {
	size = ALIGNUP(size);
	
	stack->size = size;
	stack->buffer = mem_alloc(size);
	stack->header = stack->buffer;
	stack->header->previous = stack->header;
	stack->header->size = 0;
}

void stack_deinit(Stack* stack) {
	mem_free(stack->buffer);
	stack->buffer = stack->header = NULL;
	stack->size = 0;
}

void* stack_push(Stack* stack, usize size) {
	size = ALIGNUP(size);
	
	struct StackHeader* previous = stack->header;
	stack->header = (void*)((char*)(previous+1) + stack->header->size);
	stack->header->previous = previous;
	
	stack->header->size = size;
	return stack->header + 1;
}

void* stack_push_zero(Stack* stack, usize size) {
	void* ptr = stack_push(stack, size);
	memset(ptr, 0, size);
	return ptr;
}

void stack_free(Stack* stack, void* ptr) {
	struct StackHeader* header = ptr;
	--header;
	
	assert(ptr >= stack->buffer && ptr < (char*)stack->buffer + stack->size);
	
	if (stack->header == header) {
		stack->header = header->previous;
	} else {
		struct StackHeader* next = (void*)((char*)ptr + header->size);
		next->previous = header->previous;
	}
}

void stack_pop(Stack* stack) {
	stack->header = stack->header->previous;
}

void stack_clear(Stack* stack) {
	stack->header = stack->buffer;
}

//~ Pool Allocator
void pool_init(MemoryPool* pool, usize chunkSize, usize chunkCount) {
	chunkSize = ALIGNUP(chunkSize);
	
	pool->size = chunkSize;
	pool->count = chunkCount;
	pool->buffer = mem_alloc(chunkSize * chunkCount);
	pool->next = NULL;
	
	pool_clear(pool);
}

void pool_deinit(MemoryPool* pool) {
	mem_free(pool->buffer);
	pool->size = 0;
	pool->count = 0;
	pool->next = pool->buffer = NULL;
}

void* pool_alloc(MemoryPool* pool) {
	struct MemoryPoolHeader* header = pool->next;
	
	if (!header) {
		assert(!"MemoryPool has no free space left!");
		return NULL;
	}
	
	pool->next = header->next;
	return header;
}

void* pool_alloc_zero(MemoryPool* pool) {
	void* ptr = pool_alloc(pool);
	memset(ptr, 0, pool->size);
	return ptr;
}

void pool_free(MemoryPool* pool, void* ptr) {
	assert(ptr >= pool->buffer && ptr < (char*)pool->buffer + (pool->count * pool->size));
	
	struct MemoryPoolHeader* header = ptr;
	
	header->next = pool->next;
	pool->next = header;
}

void pool_clear(MemoryPool* pool) {
	struct MemoryPoolHeader* header;
	
	for (usize i = 0; i < pool->count; ++i) {
		header = (void*)((char*)pool->buffer + pool->size * i);
		
		header->next = pool->next;
		pool->next = header;
	}
}

