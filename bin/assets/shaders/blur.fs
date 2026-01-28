precision mediump float;

layout(binding = 0)  uniform sampler2D inputTexture;
uniform vec4 texelSize;
in vec2 textureCoordinate;
out vec4 fragColor;

void main() {
    vec4 color = texture(inputTexture, textureCoordinate) * 0.2270;
    vec2 offset12 = 1.3834 * texelSize.xy;
    color += texture(inputTexture, textureCoordinate + offset12) * 0.3156;
    color += texture(inputTexture, textureCoordinate - offset12) * 0.3156;
    vec2 offset34 = 3.2308 * texelSize.xy;
    color += texture(inputTexture, textureCoordinate + offset34) * 0.0702;
    color += texture(inputTexture, textureCoordinate - offset34) * 0.0702;
    fragColor = color;
}