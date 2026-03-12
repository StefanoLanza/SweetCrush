precision mediump float;

layout(binding = 0)  uniform sampler2D sourceTexture;
uniform vec4 texelSize;
in vec2 vTexCoord;
out vec4 fragColor;

void main() {
	// 8-tap upsample (2x2 grid of bilinear samples)
    vec4 sum = vec4(0.0);
    
    // Closer taps
    sum += texture(sourceTexture, vTexCoord + vec2(-1.0, -1.0) * texelSize.xy * 0.5) * 2.0;
    sum += texture(sourceTexture, vTexCoord + vec2( 1.0, -1.0) * texelSize.xy * 0.5) * 2.0;
    sum += texture(sourceTexture, vTexCoord + vec2(-1.0,  1.0) * texelSize.xy * 0.5) * 2.0;
    sum += texture(sourceTexture, vTexCoord + vec2( 1.0,  1.0) * texelSize.xy * 0.5) * 2.0;

    // Wider taps
    sum += texture(sourceTexture, vTexCoord + vec2(-1.0,  0.0) * texelSize.xy);
    sum += texture(sourceTexture, vTexCoord + vec2( 1.0,  0.0) * texelSize.xy);
    sum += texture(sourceTexture, vTexCoord + vec2( 0.0, -1.0) * texelSize.xy);
    sum += texture(sourceTexture, vTexCoord + vec2( 0.0,  1.0) * texelSize.xy);
    fragColor = sum * 0.08333; // 1.0 / 12.0
}