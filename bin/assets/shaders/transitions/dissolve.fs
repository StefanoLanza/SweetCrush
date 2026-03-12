precision mediump float; // needed for hash

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 progress; // 0.0 to 1.0
out vec4 fragColor;

float hash(highp vec2 co) {
	return fract(sin(dot(co ,vec2(12.9898,78.233))) * 43758.5453); // range [0,1]
}

void main() {
	const float gridSize = 64.0;
	vec2 tile = floor((gl_FragCoord.xy - 0.5) / gridSize);
	float r = hash(tile);
	vec4 tex0 = texture(texture0, vTexCoord);
	vec4 tex1 = texture(texture1, vTexCoord);
	if (progress.x < 0.5) {
		// dissolve snapshot to black
		float t = progress.x * 2.0 - r;
		fragColor =  mix(tex1, vec4(0.), smoothstep(-0.25, 0.25, t));
	} else {
		// dissolve from black to new image
		float t = (progress.x - 0.5) * 2.0 - r;
		fragColor =  mix(vec4(0.), tex0, smoothstep(-0.25, 0.25, t));
	}
}
