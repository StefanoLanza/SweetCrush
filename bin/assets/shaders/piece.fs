precision mediump float;

layout(binding = 0) uniform sampler2D colorTexture;
layout(binding = 1) uniform sampler2D maskTexture;
uniform vec4 color;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
	vec4 pieceColor = color * texture(colorTexture, textureCoordinate.xy, 0.);
	float mask = texture(maskTexture, textureCoordinate.xy, 0.).x;
    fragColor = pieceColor;	
	mask *= smoothstep(0.05, 0.1, dot(vec3(0.33), pieceColor.xyz));
	fragColor.xyz = mix(pieceColor.xyz, vec3(1.0), mask);
}
