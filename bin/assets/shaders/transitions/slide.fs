precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 misc; // dir, progress, border thickness
uniform vec4 color0;
out vec4 fragColor;

void main() {
	vec2 uv0 = vTexCoord;
	vec2 uv1 = vTexCoord;
	uv0.x += misc.x * (1.0 - misc.y);
	uv1.x += misc.x * (-misc.y);
    vec4 tex0 = texture(texture0, uv0);
    vec4 tex1 = texture(texture1, uv1);
	tex0 = mix(tex0, color0, abs(uv0.x - 0.5) > 0.5); // make out of bounds black
    fragColor =  mix(tex0, tex1, (abs(uv0.x - 0.5) < (0.5 + misc.z)) ? 0.0 : 1.0);
}
