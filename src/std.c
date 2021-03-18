#include "headers/std.h"

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
