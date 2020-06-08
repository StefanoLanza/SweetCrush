#version 300 es

precision mediump float;

uniform sampler2D inputTexture;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
    fragColor = texture(inputTexture, textureCoordinate);
	if (abs(textureCoordinate.x - 0.5) > 0.5 || abs(textureCoordinate.y - 0.5) > 0.5) {
		fragColor = vec4(0., 0., 0., 0.);
	}
}
