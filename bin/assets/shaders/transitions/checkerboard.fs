precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 progress; // 0.0 to 1.0
out vec4 fragColor;

// a must be odd
uint permute32(uint i, uint a = 17u, uint b = 12357u) {
    return (a * i + b) & 31u;
}

void main() {
	const uint gridSize = 64u;
	uvec2 tile = uvec2(gl_FragCoord.xy - 0.5) / gridSize;
	uint tileIdx = tile.x + tile.y * 32u;
	tileIdx = permute32(tileIdx);

    vec4 color0 = texture(texture0, vTexCoord);
    vec4 color1 = texture(texture1, vTexCoord);
    fragColor =  mix(color1, color0, progress.x > (float(tileIdx) / 32.0)); // checkerboard pattern
}
