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

// Let the compiler do it's job
int printf(const char* restrict, ...);

/// Structs
// A range of characters in memory. Not necessarily null-terminated.
typedef struct string {
	usize len;
	const char* ptr;
} string;

// A dynamic null-terminated buffer of characters.
typedef struct strbuf {
	usize cap, len;
	char data[];
} strbuf;

// Make a 'struct string' of a string literal
#define strlit(s) ((string){ .len = sizeof(s)-sizeof((s)[0]), .ptr = (s) })

// Dynamic List of T
#define List(T) struct { usize cap, len; T* ptr; }
typedef List(void) List_generic; // Basic type for functions

#define list_sizeof_item(list) (sizeof ((list)->ptr[0]))
#define list_init(list, cap) (__list_init((List_generic)(list), (cap), list_sizeof_item(list)))
#define list_reserve(list, newcap) (__list_reserve((List_generic)(list), (newcap), list_sizeof_item(list)))
#define list_push(list, item) (list_reserve(list, (list)->len + 1), (list)->ptr[(list)->len++] = (item))
#define list_pop(list) ((list)->ptr[--(list)->len])
#define list_free(list) (mem_free((list)->ptr), (list)->ptr = NULL, (list)->len = (list)->cap = 0)

// More math stuff
internal inline f32 lerpf(f32 a, f32 b, f32 t) {
	return (b - a) * t + a;
}

internal inline f32 lerp(f64 a, f64 b, f64 t) {
	return (b - a) * t + a;
}

#ifndef __CREATE_IMPL__

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

#else
internal usize usize_min(usize a, usize b) {
	if (a > b)
		return b;
	return a;
}

internal usize usize_max(usize a, usize b) {
	if (a > b)
		return a;
	return b;
}

// 'struct string' functions
string string_from_cstr_len(const char* restrict ptr, usize len) {
	return (string) {
		.len = len,
		.ptr = ptr
	};
}

string string_from_cstr(const char* restrict ptr) {
	return (string) {
		.len = strlen(ptr),
		.ptr = ptr
	};
}

string string_from_strbuf(const strbuf* restrict buf) {
	return (string) {
		.len = buf->len,
		.ptr = buf->data
	};
}

string string_from_strbuf_slice(const strbuf* restrict buf, usize begin, usize count) {
	return (string) {
		.len = count,
		.ptr = buf->data + begin
	};
}

string string_copy_from_strbuf(const strbuf* restrict buf) {
	char* ptr = mem_alloc((buf->len + 1) * sizeof(char));
	
	memcpy(ptr, buf->data, buf->len * sizeof(char));
	ptr[buf->len] = 0;
	
	return (string) {
		.len = buf->len,
		.ptr = ptr
	};
}

string string_copy(string other) {
	char* ptr = mem_alloc((other.len + 1) * sizeof(char));
	
	memcpy(ptr, other.ptr, other.len * sizeof(char));
	ptr[other.len] = 0;
	
	return (string) {
		.len = other.len,
		.ptr = ptr
	};
}

void string_free(string str) {
	mem_free((void*)str.ptr);
}

int string_compare(string a, string b) {
	return strncmp(a.ptr, b.ptr, usize_max(a.len, b.len));
}

int string_compare_range(string a, usize begin_a, string b, usize begin_b, usize count) {
	return strncmp(a.ptr + begin_a, b.ptr + begin_b, count);
}

// 'struct strbuf' functions
strbuf* strbuf_make(usize cap) {
	strbuf* result = mem_alloc(sizeof(strbuf) + (cap+1) * sizeof(char));
	result->cap = cap;
	result->len = 0;
	result->data[0] = 0;
	
	return result;
}

void strbuf_free(strbuf* restrict buf) {
	mem_free(buf);
}

usize strbuf_reserve(strbuf* restrict* restrict buf, usize newcap) {
	if ((*buf)->cap >= newcap)
		return (*buf)->cap;
	
	strbuf* newbuf = mem_realloc(*buf, sizeof(strbuf) + (newcap + 1) * sizeof(char));
	newbuf->cap = newcap;
	*buf = newbuf;
	
	return newbuf->cap;
}

usize strbuf_append(strbuf* restrict* restrict buf, string str) {
	usize desired_length = (*buf)->len + str.len;
	
	if (strbuf_reserve(buf, desired_length) < desired_length)
		return (*buf)->len;
	
	memcpy((*buf)->data + (*buf)->len, str.ptr, str.len * sizeof(char));
	(*buf)->len += str.len;
	(*buf)->data[desired_length] = 0;
	
	return desired_length;
}

usize strbuf_append_cstr(strbuf* restrict* restrict buf, const char* restrict str) {
	return strbuf_append(buf, string_from_cstr(str));
}

usize strbuf_append_strbuf(strbuf* restrict* restrict buf, const strbuf* restrict other) {
	return strbuf_append(buf, string_from_strbuf(other));
}

strbuf* strbuf_copy(const strbuf* restrict buf) {
	strbuf* result = strbuf_make(buf->cap);
	memcpy(result->data, buf->data, buf->len);
	result->len = buf->len;
	result->data[result->len] = 0;
	
	return result;
}

void strbuf_rewind(strbuf* restrict buf) {
	buf->len = 0;
	buf->data[0] = 0;
}

usize strbuf_rollback(strbuf* restrict buf, usize amount) {
	if (amount >= buf->len) {
		buf->len = 0;
		buf->data[0] = 0;
		return 0;
	}
	
	buf->len -= amount;
	buf->data[buf->len] = 0;
	return buf->len;
}

usize strbuf_weak_append(strbuf* restrict* restrict buf, string str) {
	usize desired_length = usize_min((*buf)->len + str.len, (*buf)->cap);
	if (desired_length <= (*buf)->len)
		return (*buf)->len;
	
	memcpy((*buf)->data + (*buf)->len, str.ptr, desired_length - (*buf)->len);
	(*buf)->len = desired_length;
	(*buf)->data[desired_length] = 0;
	return desired_length;
}

usize strbuf_weak_append_cstr(strbuf* restrict* restrict buf, const char* restrict str) {
	return strbuf_weak_append(buf, string_from_cstr(str));
}

usize strbuf_weak_append_strbuf(strbuf* restrict* restrict buf, const strbuf* restrict other) {
	return strbuf_weak_append(buf, string_from_strbuf(other));
}

// List
void __list_init(List_generic* list, usize cap, usize size) {
	list->ptr = mem_alloc(size * cap);
	list->len = 0;
	list->cap = cap;
}

usize __list_reserve(List_generic* list, usize newcap, usize size) {
	if (list->cap >= newcap)
		return list->cap;
	
	void* newptr = mem_realloc(list->ptr, size * newcap);
	if (!newptr)
		return list->cap;
	
	list->ptr = newptr;
	list->cap = newcap;
	
	return newcap;
}
#endif
