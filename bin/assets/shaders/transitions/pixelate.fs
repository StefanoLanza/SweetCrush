precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 uvRect0;
uniform vec4 uvRect1;
uniform vec4 color0;
uniform vec4 color1;
uniform vec4 progress; // 0.0 to 1.0
out vec4 fragColor;

void main() {
	vec2 uv0 = uvRect0.xy + uvRect0.zw * vTexCoord;
	vec2 uv1 = uvRect1.xy + uvRect1.zw * vTexCoord;
    vec4 tex0 = texture(texture0, uv0);
    vec4 tex1 = texture(texture1, uv1);
    fragColor =  mix(tex0, tex1, (abs(uv0.x - 0.5) < 0.5) ? 0.0 : 1.0) + progress.x * 0.0001;
}
