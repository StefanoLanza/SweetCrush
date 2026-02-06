precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 misc; // dirx, diry, progress, border thickness
out vec4 fragColor;

void main() {
	vec2 uv0 = vTexCoord + misc.xy * (1.0 - misc.z);
	vec2 uv1 = vTexCoord + misc.xy * (-misc.z);
    vec4 tex0 = texture(texture0, uv0);
    vec4 tex1 = texture(texture1, uv1);
	float borderTest = step(abs(uv0.x - 0.5), 0.5 + misc.w) * step(abs(uv0.y - 0.5), 0.5 + misc.w); // add border
    fragColor =  mix(tex1, tex0, borderTest);
}
