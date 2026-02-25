precision mediump float;

layout(binding = 0)  uniform sampler2D inputTexture;
uniform vec4 texelSize;
uniform vec4 gridSize;
in vec2 textureCoordinate;
out vec4 fragColor;

void main() {
	vec2 normalizedPixelSize = gridSize.x * texelSize.xy;
	vec2 uvPixel = normalizedPixelSize * floor(textureCoordinate / normalizedPixelSize); // TODO Avoid div
    vec4 color = texture(inputTexture, uvPixel);

	float luma = dot(vec3(0.2126, 0.7152, 0.0722), color.rgb);
	float radius = gridSize.y * (0.1 + luma);

	vec2 cellUv = fract(textureCoordinate / normalizedPixelSize);
	float dist = length(cellUv - 0.5);  
  	float circle = smoothstep(radius - 0.01, radius + 0.01, dist);
    fragColor = mix(color, vec4(0.0, 0.0, 0.0, 1.0), circle);
}