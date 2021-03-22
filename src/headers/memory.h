#pragma once

#include "types.h"

void* mem_alloc(usize size);
void* mem_alloc_zero(usize count, usize size);
void* mem_realloc(void* p, usize size);
void* mem_reallod_zero(void* p, usize count, usize size);
void mem_free(void* p);

// Arena Allocator
// All alocations made in _arena will be reversed at the end of the scope.
// NOTE(luigi): be careful with breaks and continues.
#define arena_scope(_arena) for(usize __arn_c=(_arena).head;__arn_c!=SIZE_MAX;(_arena).head=__arn_c,__arn_c=SIZE_MAX)

// To be used in loops. Manual 'arena_scope_end', so break and continue work as expected.
#define arena_scope_begin(_arena) usize __arn_c=(_arena).head
#define arena_scope_end(_arena) ((_arena).head=__arn_c)

typedef struct Arena Arena;
struct Arena {
	usize head;
	usize size;
	void* buffer;
};

void arena_init(Arena* restrict arena, usize size);
void arena_deinit(Arena* restrict arena);
void* arena_alloc(Arena* restrict arena, usize size);
void* arena_alloc_zero(Arena* restrict arena, usize size);
void arena_clear(Arena* restrict arena);

// Stack Allocator
typedef struct Stack Stack;
struct Stack {
	usize size;
	struct StackHeader* header;
	void* buffer;
};

struct StackHeader {
	struct StackHeader* previous;
	usize size;
};

void stack_init(Stack* stack, usize size);
void stack_deinit(Stack* stack);
void* stack_push(Stack* stack, usize size);
void* stack_push_zero(Stack* stack, usize size);
void stack_free(Stack* stack, void* ptr);
void stack_pop(Stack* stack);
void stack_clear(Stack* stack);

// Pool Allocator
typedef struct MemoryPool MemoryPool;
struct MemoryPool {
	int a;
};

void mempool_init(MemoryPool* pool);
void mempool_deinit(MemoryPool* pool);




