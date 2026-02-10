precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 misc; // dirx, diry, progress, border thickness
out vec4 fragColor;

void main() {
	const float displ = 1.0;
	vec2 uv0 = vTexCoord;// + misc.xy * (1.0 - misc.z) * displ;
	vec2 uv1 = vTexCoord + misc.xy * (-misc.z) * displ;
	float b0 = step(max(abs(uv0.x - 0.5), abs(uv0.y - 0.5)), 0.5); // add border
	float b1 = step(max(abs(uv1.x - 0.5), abs(uv1.y - 0.5)), 0.5); // add border
	float b2 = step(max(abs(uv1.x - 0.5), abs(uv1.y - 0.5)), 0.525); // add border
    vec4 tex0 = texture(texture0, uv0);
    vec4 tex1 = texture(texture1, uv1);
	tex1 = mix(vec4(0.), tex1,  b1);
    fragColor =  mix(tex0, tex1, b2);
}
