precision mediump float;

uniform sampler2D inputTexture;
uniform vec4 color;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
	vec4 color1 = color;
	float alpha = 1.0;
	color1.xyz *= alpha;
	fragColor = color1 * texture(inputTexture, vec2(0.5, textureCoordinate.y), 0.);  // additive
}
