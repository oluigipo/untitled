#pragma once
#include "types.h"

// NOTE(luigi): if any memory allocation with those functions fail, a message
//              box will popup to the user telling what occured, and then exit
//              the application.
void* mem_alloc(usize size);
void* mem_alloc_zero(usize count, usize size);
void* mem_realloc(void* p, usize size);
void* mem_reallod_zero(void* p, usize count, usize size);
void mem_free(void* p);

//~ Arena Allocator
typedef struct Arena Arena;
struct Arena {
	usize head;
	usize size;
	void* buffer;
};

func void arena_init(Arena* restrict arena, usize size);
func void arena_deinit(Arena* restrict arena);
func void* arena_alloc(Arena* restrict arena, usize size);
func void* arena_alloc_zero(Arena* restrict arena, usize size);
func void arena_clear(Arena* restrict arena);

//~ Stack Allocator
typedef struct Stack Stack;
struct Stack {
	usize size;
	struct StackHeader* header;
	void* buffer;
};

struct StackHeader {
	struct StackHeader* previous;
	usize size;
}
#if X86
__attribute__((aligned(16)))
#endif
;

func void stack_init(Stack* stack, usize size);
func void stack_deinit(Stack* stack);
func void* stack_push(Stack* stack, usize size);
func void* stack_push_zero(Stack* stack, usize size);
#define stack_alloc stack_push
#define stack_alloc_zero stack_push_zero
func void stack_free(Stack* stack, void* ptr);
func void stack_pop(Stack* stack);
func void stack_clear(Stack* stack);

//~ Pool Allocator
typedef struct MemoryPool MemoryPool;
struct MemoryPool {
	usize count;
	usize size;
	void* buffer;
	struct MemoryPoolHeader* next;
};

struct MemoryPoolHeader {
	struct MemoryPoolHeader* next;
}
#if X86
__attribute__((aligned(16)))
#endif
;

func void pool_init(MemoryPool* pool, usize chunkSize, usize chunkCount);
func void pool_deinit(MemoryPool* pool);
func void* pool_alloc(MemoryPool* pool);
func void* pool_alloc_zero(MemoryPool* pool);
func void pool_free(MemoryPool* pool, void* ptr);
func void pool_clear(MemoryPool* pool);

