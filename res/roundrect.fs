#version 330
#define PI 3.141592653589793238462643383279

in vec2 vPosition;

out vec4 oFragColor;

uniform vec2 uScale;
uniform vec4 uColor;
uniform float uRoundness;

float calculate_roundrect_distance(vec2 p, vec4 r) {
	vec2 b = uScale;
	
    r.xy = (p.x > 0.0) ? r.xy : r.zw;
    r.x  = (p.y > 0.0) ? r.x  : r.y;
    
    vec2 q = abs(p * (uScale)) - b + r.x;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r.x;
}

void main() {
	vec4 r = 0.3 + 0.3 * cos(vec4(PI - uRoundness));
	float d = calculate_roundrect_distance(vPosition * 2.0 - 1.0, r);
	
	d = max(-sign(d), 0.0);
	
	oFragColor = uColor * d;
}
