precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 misc; // dir, progress, border thickness
uniform vec4 fadeColor;
out vec4 fragColor;

void main() {
	vec2 uv0 = vTexCoord;
	vec2 uv1 = vTexCoord;
	uv0.x += misc.x * (1.0 - misc.y);
	uv1.x += misc.x * (-misc.y);
	float isInBounds = step(0.0, uv0.x) * step(uv0.x, 1.0);
	//float isInBounds = step(abs(uv0.x - 0.5), 0.5);
    vec4 tex0 = texture(texture0, uv0);
    vec4 tex1 = texture(texture1, uv1);
	tex0 = mix(fadeColor, tex0, isInBounds); // make out of bounds black
	float borderTest = step(0.5 + misc.z, abs(uv0.x - 0.5));
    fragColor =  mix(tex0, tex1, borderTest);
}
