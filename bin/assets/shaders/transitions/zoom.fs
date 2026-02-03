precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 misc; // dir, progress
uniform vec4 color0;
out vec4 fragColor;

void main() {
	vec2 uv0 = vTexCoord;
	vec2 uv1 = vTexCoord;
	// Shrink towards (misc.x == 1) or expand from (misc.x == -1) center
	uv1 = (uv1 - 0.5) * (1.0 + misc.x * misc.y) + 0.5;
	uv0 = (uv0 - 0.5) * (1.0 - misc.x * (1. - misc.y)) + 0.5;
	float l = float( (abs(uv1.x - 0.5) > 0.5) || (abs(uv1.y - 0.5) > 0.5) ) ;
    vec4 tex0 = texture(texture0, uv0);
    vec4 tex1 = texture(texture1, uv1);
	//tex0 = mix(tex0, color0, abs(uv0.x - 0.5) > 0.5); // make out of bounds black
    fragColor =  mix(tex1, tex0, l);
}
