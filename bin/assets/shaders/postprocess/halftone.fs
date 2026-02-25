precision highp float;

layout(binding = 0)  uniform sampler2D inputTexture;
uniform vec4 texelSize;
uniform vec4 gridSize;
in vec2 textureCoordinate;
out vec4 fragColor;

void main() {
	vec2 normalizedPixelSize = gridSize.x * texelSize.xy;

	vec2 offsetUv = textureCoordinate;
	float rowIndex = floor(textureCoordinate.y / normalizedPixelSize.y);
	if (mod(rowIndex, 2.0) == 1.0) {
		offsetUv.x += normalizedPixelSize.x * 0.5;
	}

	vec2 uvPixel = normalizedPixelSize * floor(offsetUv / normalizedPixelSize); // TODO Avoid div
	mediump vec4 color = texture(inputTexture, uvPixel);

	mediump float luma = dot(vec3(0.2126, 0.7152, 0.0722), color.rgb);
	mediump float radius = gridSize.y * (0.1 + luma);


	vec2 cellUv = fract(offsetUv / normalizedPixelSize);
	float dist = length(cellUv - 0.5);  

	mediump float circleMask = smoothstep(radius, radius - 0.05, dist);
    fragColor = color * circleMask;
}