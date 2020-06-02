#version 300 es

precision mediump float;

uniform sampler2D inputTexture;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
    fragColor = texture(inputTexture, textureCoordinate);
}
