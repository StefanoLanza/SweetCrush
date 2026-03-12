precision mediump float;

uniform sampler2D inputTexture;
in vec4 color;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
	fragColor = color * texture(inputTexture, textureCoordinate.xy, 0.);
}
