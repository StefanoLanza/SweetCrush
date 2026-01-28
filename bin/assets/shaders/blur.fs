precision mediump float;

layout(binding = 0)  uniform sampler2D inputTexture;
uniform vec4 srcTexelSize;
in vec2 textureCoordinate;
out vec4 fragColor;

void main() {
	vec2 samplePos = textureCoordinate * srcTexelSize.xy;
	vec4 color = vec4(0.0);
	for (int i = -2; i <= 2; i++) {
		vec2 uv = samplePos + float(i) * srcTexelSize.zw;
		color += texture(inputTexture, uv, 0.0);
	}
	color /= 5.0;
	fragColor = color;
}
