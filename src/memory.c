//#define MEM_TRACK_HEAP_ALLOCATIONS

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
	assert(false);
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

//~ Arena Allocator
// All alocations made in _arena will be reversed at the end of the scope.
// NOTE(luigi): be careful with breaks and continues.
#define arena_scope(_arena) for(usize __arn_c=(_arena).head;__arn_c!=SIZE_MAX;(_arena).head=__arn_c,__arn_c=SIZE_MAX)

// To be used in loops. Manual 'arena_scope_end', so break and continue work as expected.
#define arena_scope_begin(_arena) usize __arn_c=(_arena).head
#define arena_scope_end(_arena) ((_arena).head=__arn_c)

struct Arena {
	usize head;
	usize size;
	void* buffer;
};

void arena_init(struct Arena* restrict arena, usize size) {
	arena->head = 0;
	arena->size = size;
	arena->buffer = mem_alloc(size);
}

void arena_deinit(struct Arena* restrict arena) {
	mem_free(arena->buffer);
	arena->head = arena->size = 0;
	arena->buffer = NULL;
}

void* arena_alloc(struct Arena* restrict arena, usize size) {
	size = (size + 7) & ~7; // 8 byte alignment
	
	if (arena->head + size > arena->size) {
		return NULL;
	}
	
	void* p = arena->buffer + arena->head;
	arena->head += size;
	return p;
}

void* arena_alloc_zero(struct Arena* restrict arena, usize size) {
	size = (size + 7) & ~7; // 8 byte alignment
	
	if (arena->head + size > arena->size) {
		return NULL;
	}
	
	void* p = arena->buffer + arena->head;
	arena->head += size;
	memset(p, 0, size);
	return p;
}

void arena_clear(struct Arena* restrict arena) {
	arena->head = 0;
}

