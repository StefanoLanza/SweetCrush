precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 misc; // progress
uniform vec4 fadeColor; // fade color
out vec4 fragColor;

void main() {
    vec4 tex0 = texture(texture0, vTexCoord);
    vec4 tex1 = texture(texture1, vTexCoord);
	tex1 = mix(tex1, fadeColor, min(1.0, misc.x * 2.0)); // fade out current
    fragColor = mix(tex1, tex0, max(0.0, (misc.x - 0.5) * 2.0)); // fade in new
}
