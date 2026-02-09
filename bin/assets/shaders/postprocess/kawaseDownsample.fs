precision mediump float;

layout(binding = 0)  uniform sampler2D sourceTexture;
uniform vec4 texelSize;
in vec2 vTexCoord;
out vec4 fragColor;

void main() {
	// 5-tap downsample (Center + 4 diagonals)
    vec4 sum = texture(sourceTexture, vTexCoord) * 4.0;
    sum += texture(sourceTexture, vTexCoord + vec2(-1.0, -1.0) * texelSize.xy);
    sum += texture(sourceTexture, vTexCoord + vec2( 1.0, -1.0) * texelSize.xy);
    sum += texture(sourceTexture, vTexCoord + vec2(-1.0,  1.0) * texelSize.xy);
    sum += texture(sourceTexture, vTexCoord + vec2( 1.0,  1.0) * texelSize.xy);

    fragColor = sum * 0.125; // (4.0 + 1+1+1+1) = 8.0, so divide by 8
}