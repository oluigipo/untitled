#include "headers/all.h"

func b32 collision_aabb(const vec4 a, const vec4 b) {
	return (a[0] + a[2] > b[0] && b[0] + b[2] > a[0])
		&& (a[1] + a[3] > b[1] && b[1] + b[3] > a[1]);
}
