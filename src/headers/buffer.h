#pragma once
#include "types.h"

// A simple ring-buffer
struct Buffer {
	u8* data;
	usize cap;
	usize head;
} typedef Buffer;

func Buffer buffer_from(u8* data, usize size);

func u8 buffer_read_u8(Buffer* buf);
func void buffer_write_u8(Buffer* buf, u8 value);
func u16 buffer_read_u16(Buffer* buf);
func void buffer_write_u16(Buffer* buf, u16 value);
func u32 buffer_read_u32(Buffer* buf);
func void buffer_write_u32(Buffer* buf, u32 value);
func u64 buffer_read_u64(Buffer* buf);
func void buffer_write_u64(Buffer* buf, u64 value);

func i8 buffer_read_i8(Buffer* buf);
func void buffer_write_i8(Buffer* buf, i8 value);
func i16 buffer_read_i16(Buffer* buf);
func void buffer_write_i16(Buffer* buf, i16 value);
func i32 buffer_read_i32(Buffer* buf);
func void buffer_write_i32(Buffer* buf, i32 value);
func i64 buffer_read_i64(Buffer* buf);
func void buffer_write_i64(Buffer* buf, i64 value);

func f32 buffer_read_f32(Buffer* buf);
func void buffer_write_f32(Buffer* buf, f32 value);
func f64 buffer_read_f64(Buffer* buf);
func void buffer_write_f64(Buffer* buf, f64 value);
