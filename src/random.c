#include "headers/random.h"
#include <stdio.h>
#include <time.h>

internal u64 random_seed[2]; // u128

internal inline u64 __rotl(const u64 x, int k) {
	return (x << k) | (x >> (64 - k));
}

// Generates a random number between 0 and U64_MAX
u64 random_u64(void) {
	const u64 s0 = random_seed[0];
	u64 s1 = random_seed[1];
	const u64 result = __rotl(s0 + s1, 17) + s0;
	
	s1 ^= s0;
	random_seed[0] = __rotl(s0, 49) ^ s1 ^ (s1 << 21); // a, b
	random_seed[1] = __rotl(s1, 28); // c
	
	return result;
}

void random_init(void) {
	static const u64 JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };
	
	random_seed[0] = time(NULL);
	random_seed[1] = time(NULL);
	
	u64 s0 = 0;
	u64 s1 = 0;
	
	for (uint i = 0; i < sizeof JUMP / sizeof *JUMP; ++i) {
		for (uint b = 0; b < 64; ++b) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= random_seed[0];
				s1 ^= random_seed[1];
			}
			random_u64();
		}
	}
	
	random_seed[0] = s0;
	random_seed[1] = s1;
}

// Generates a random number between 0 and 1
f64 random_f64(void) {
	return (f64)random_u64() / U64_MAX;
}

// 32 bits version of random()
u32 random_u32(void) {
	u64 result = random_u64();
	return (u32)(result >> 32); // the higher bits are more random.
}

// Test the randomness of random_f64()
void random_test(void) {
	FILE* file = fopen("random_test.ppm", "w");
	if (!file)
		return;
	
	fprintf(file, "P3 256 256 255 ");
	const uint total = 256*256*3;
	for (uint i = 0; i < total; ++i) {
		fprintf(file, "%i ", (int)(random_f64() * 256));
	}
	
	fclose(file);
}

