precision mediump float;

uniform sampler2D inputTexture;
uniform vec4 color;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
	fragColor = color * texture(inputTexture, vec2(0.5, textureCoordinate.y), 0.);  // additive
}
