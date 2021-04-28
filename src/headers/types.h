#pragma once
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#define true 1
#define false 0
#define internal static
#define global extern
#define func static

#define restrict
#undef restrict

#ifndef NULL
#define NULL 0
#endif

typedef unsigned int uint;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef i8 b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

typedef float f32;
typedef double f64;

typedef size_t usize;
typedef ptrdiff_t isize;

#define U8_MIN 0
#define U8_MAX UCHAR_MAX
#define U16_MIN 0
#define U16_MAX USHRT_MAX
#define U32_MIN 0
#define U32_MAX UINT_MAX
#define U64_MIN 0
#define U64_MAX ULLONG_MAX

#define I8_MIN SCHAR_MIN
#define I8_MAX SCHAR_MAX
#define I16_MIN SHRT_MIN
#define I16_MAX SHRT_MAX
#define I32_MIN INT_MIN
#define I32_MAX INT_MAX
#define I64_MIN LLONG_MIN
#define I64_MAX LLONG_MAX

#define USIZE_MIN 0
#define USIZE_MAX SIZE_MAX

#define ISIZE_MIN PTRDIFF_MIN
#define ISIZE_MAX PTRDIFF_MAX

#define PI32 ((f32)PI64)
#define PI64 3.141592653589793238462643383279
#define PI PI64

#define TAU32 ((f32)PI32*2)
#define TAU64 (PI64*2)
#define TAU TAU64

