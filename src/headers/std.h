#pragma once
#include "types.h"
#include "debug.h"
#include "os.h"
#include "memory.h"

// Includes
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <time.h>

#ifdef MSVC
#  define likely(x) x
#  define unlikely(x) x
#else
#  define likely(x) __builtin_expect((x) != 0, true)
#  define unlikely(x) __builtin_expect((x) != 0, false)
#endif

#define gigabytes(x) (megabytes(x) * 1024ull)
#define megabytes(x) (kilobytes(x) * 1024ull)
#define kilobytes(x) ((u64)(x) * 1024ull)

#ifndef offsetof
#  define offsetof(T,f) ((char*)(&((T*)0)->f))
#endif

#define strfmt(s) (s).len, (s).ptr

/// Structs
// A range of characters in memory. Not necessarily null-terminated.
typedef struct string {
	usize len;
	const unsigned char* ptr;
} string;

typedef struct stringUTF8 {
	usize size;
	const unsigned char* ptr;
} stringUTF8;

// Returns str's length. 0 will be returned if some error occured
usize string_utf8_len(stringUTF8 str);
// Returns the char at position 'index' in 'str'. 0 will be returned if some error occured
u32 string_utf8_char_at(stringUTF8 str, usize index);
// Returns the next char of iterator 'str'. 0 will be returned and 'str' won't be modified if some error occured
u32 string_utf8_next_char(stringUTF8* str);
// Encode 'ch' in 'str'. Returns the number of bytes written to 'str'. 0 will be returned if the size of bytes won't fit in 'str' (of size 'max') or any other error occurs.
usize string_utf8_write_char(u8* str, usize max, u32 ch);

// A dynamic null-terminated buffer of characters.
typedef struct strbuf {
	usize cap, len;
	unsigned char data[];
} strbuf;

// Make a 'struct string' of a string literal
#define strlit(s) ((string){ .len = sizeof(s)-sizeof((s)[0]), .ptr = (s) })

// Dynamic List of T
#define List(T) struct { usize cap, len; T* ptr; }
typedef List(void) List_generic; // Basic type for functions

#define list_sizeof_item(list) (sizeof ((list)->ptr[0]))
#define list_init(list, cap) (__list_init((List_generic*)(list), (cap), list_sizeof_item(list)))
#define list_reserve(list, newcap) (__list_reserve((List_generic*)(list), (newcap), list_sizeof_item(list)))
#define list_push(list, item) (list_reserve(list, (list)->len + 1), (list)->ptr[(list)->len++] = (item))
#define list_pop(list) ((list)->ptr[--(list)->len])
#define list_free(list) (mem_free((list)->ptr), (list)->ptr = NULL, (list)->len = (list)->cap = 0)

// More math stuff
internal inline f32 lerpf(f32 a, f32 b, f32 t) {
	return b * t + a * (1-t);
}

internal inline f32 lerp(f64 a, f64 b, f64 t) {
	return b * t + a * (1-t);
}

// Hashing. Both of them are case insensetive.
u64 hash_of_cstr(const char* str);
u64 hash_of_str(string str);

/// string
string string_from_cstr_len(const char* restrict ptr, usize len);
string string_from_cstr(const char* restrict ptr);
string string_from_strbuf(const strbuf* restrict buf);
string string_from_strbuf_slice(const strbuf* restrict buf, usize begin, usize end);
string string_copy_from_strbuf(const strbuf* restrict buf);
string string_copy(string other);

// Same as 'strncmp'
int string_compare(string a, string b);
int string_compare_range(string a, usize begin_a, string b, usize begin_b, usize count);

// Same as 'free'
void string_free(string str);

/// strbuf
// Allocates a new strbuf.
strbuf* strbuf_make(usize cap);
// Deallocates a strbuf.
void strbuf_free(strbuf* restrict buf);
// Reallocates 'buf' if it's capacity doesn't fit 'newcap'
usize strbuf_reserve(strbuf* restrict* restrict buf, usize newcap);
// Allocates a copy of a strbuf.
strbuf* strbuf_copy(const strbuf* restrict buf);
// Clear 'buf', so it's length is 0.
void strbuf_rewind(strbuf* restrict buf);
// Clear the last 'amount' characters of 'buf'. Rollback N amount of characters.
usize strbuf_rollback(strbuf* restrict buf, usize amount);

// Appends text to the end of 'buf', and maybe reallocates if it doesn't fit.
usize strbuf_append(strbuf* restrict* restrict buf, string str);
usize strbuf_append_cstr(strbuf* restrict* restrict buf, const char* restrict str);
usize strbuf_append_strbuf(strbuf* restrict* restrict buf, const strbuf* restrict other);

// Appends text to the end of 'buf', but it will cut the text so it fits within
// the capacity.
usize strbuf_weak_append(strbuf* restrict* restrict buf, string str);
usize strbuf_weak_append_cstr(strbuf* restrict* restrict buf, const char* restrict str);
usize strbuf_weak_append_strbuf(strbuf* restrict* restrict buf, const strbuf* restrict other);

// List
void __list_init(List_generic* list, usize cap, usize size);
usize __list_reserve(List_generic* list, usize newcap, usize size);
