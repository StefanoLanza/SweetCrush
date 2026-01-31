precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 uvRect0;
uniform vec4 uvRect1;
uniform vec4 progress; // 0.0 to 1.0
out vec4 fragColor;

void main() {
    vec4 color0 = texture(texture0, uvRect0.xy + uvRect0.zw * vTexCoord);
    vec4 color1 = texture(texture1, uvRect1.xy + uvRect1.zw * vTexCoord);
    fragColor =  mix(color1, color0, progress.x); // > (float(tileIdx) / 32.0)); // checkerboard pattern
}
