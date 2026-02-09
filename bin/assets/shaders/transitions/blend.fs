precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    vec4 tex0 = texture(texture0, vTexCoord);
    vec4 tex1 = texture(texture1, vTexCoord);
    fragColor = mix(tex1, tex0, tex0.w); // simple alpha blend
}
