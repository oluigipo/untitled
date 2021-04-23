#include "headers/all.h"
#include <stdlib.h>

Buffer buffer_from(u8* data, usize size) {
	return (Buffer) {
		.data = data,
		.cap = size,
		.head = 0
	};
}

#define BUF_READ(T) T buffer_read_##T(Buffer* buf) { T result = 0; memcpy(&result, buf->data + buf->head, sizeof result); buf->head = (buf->head + sizeof result) % buf->cap; return result; }
#define BUF_WRITE(T) void buffer_write_##T(Buffer* buf, T value) { memcpy(buf->data + buf->head, &value, sizeof value); buf->head = (buf->head + sizeof value) % buf->cap; }

BUF_READ(u8) BUF_WRITE(u8)
BUF_READ(u16) BUF_WRITE(u16)
BUF_READ(u32) BUF_WRITE(u32)
BUF_READ(u64) BUF_WRITE(u64)

BUF_READ(i8) BUF_WRITE(i8)
BUF_READ(i16) BUF_WRITE(i16)
BUF_READ(i32) BUF_WRITE(i32)
BUF_READ(i64) BUF_WRITE(i64)

BUF_READ(f32) BUF_WRITE(f32)
BUF_READ(f64) BUF_WRITE(f64)

#undef BUF_READ
#undef BUF_WRITE
