#pragma once
#include "types.h"

// A simple ring-buffer
struct Buffer {
	u8* data;
	usize cap;
	usize head;
} typedef Buffer;

Buffer buffer_from(u8* data, usize size);

u8 buffer_read_u8(Buffer* buf);
void buffer_write_u8(Buffer* buf, u8 value);
u16 buffer_read_u16(Buffer* buf);
void buffer_write_u16(Buffer* buf, u16 value);
u32 buffer_read_u32(Buffer* buf);
void buffer_write_u32(Buffer* buf, u32 value);
u64 buffer_read_u64(Buffer* buf);
void buffer_write_u64(Buffer* buf, u64 value);

i8 buffer_read_i8(Buffer* buf);
void buffer_write_i8(Buffer* buf, i8 value);
i16 buffer_read_i16(Buffer* buf);
void buffer_write_i16(Buffer* buf, i16 value);
i32 buffer_read_i32(Buffer* buf);
void buffer_write_i32(Buffer* buf, i32 value);
i64 buffer_read_i64(Buffer* buf);
void buffer_write_i64(Buffer* buf, i64 value);

f32 buffer_read_f32(Buffer* buf);
void buffer_write_f32(Buffer* buf, f32 value);
f64 buffer_read_f64(Buffer* buf);
void buffer_write_f64(Buffer* buf, f64 value);
