precision mediump float;

uniform sampler2D inputTexture;
uniform vec4 color;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
	vec4 color1 = color;
	color1.a *= 1.0 - pow(1.0 - textureCoordinate.x, 4.0);
	fragColor = color1; // * texture(inputTexture, textureCoordinate.xy, 0.);
}
