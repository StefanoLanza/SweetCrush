precision mediump float;

layout(binding = 0)  uniform sampler2D inputTexture;
in vec2 textureCoordinate;
out vec4 fragColor;

void main() {
	fragColor = texture(inputTexture, textureCoordinate);
}
