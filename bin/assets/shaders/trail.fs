precision mediump float;

uniform sampler2D inputTexture;
uniform vec4 color;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
	vec4 color1 = color;
	float alpha = 1.0 - pow(1.0 - textureCoordinate.x, 4.0);
	alpha *= 1.0 - smoothstep(0.8, 1.0, textureCoordinate.x);
	fragColor = color1 * texture(inputTexture, textureCoordinate.xy, 0.) * alpha;  // additive
}
