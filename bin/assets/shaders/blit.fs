precision mediump float;

uniform sampler2D inputTexture;
uniform vec4 srcTexelSize;
in vec2 textureCoordinate;
out vec4 fragColor;

vec4 cubic(float x) {
    float x2 = x * x;
    float x3 = x2 * x;
    vec4 w;
    w.x = -0.5 * x3 + x2 - 0.5 * x;
    w.y =  1.5 * x3 - 2.5 * x2 + 1.0;
    w.z = -1.5 * x3 + 2.0 * x2 + 0.5 * x;
    w.w =  0.5 * x3 - 0.5 * x2;
    return w;
}

void main() {
#if 1
	fragColor = texture(inputTexture, textureCoordinate);
#elif 0
	vec2 samplePos = textureCoordinate * srcTexelSize.xy - 0.5;
    vec2 f = fract(samplePos);
    vec2 p0 = floor(samplePos);
    vec4 xWeights = cubic(f.x);
    vec4 yWeights = cubic(f.y);
    vec4 color = vec4(0.0);
    for (int y = -1; y <= 2; y++) {
        float weightY = yWeights[y + 1];
        for (int x = -1; x <= 2; x++) {
            float weightX = xWeights[x + 1];            
            vec2 uv = (p0 + vec2(float(x), float(y)) + 0.5) * srcTexelSize.zw;
            color += texture(inputTexture, uv) * (weightX * weightY);
        }
    }
	fragColor = color;
#else
	vec2 samplePos = textureCoordinate * srcTexelSize.xy - 0.5;
    vec2 f = fract(samplePos);
    vec2 p0 = floor(samplePos);
	// Catmull-Rom weights
    vec2 f2 = f * f;
    vec2 f3 = f2 * f;

    vec2 w0 = f2 - 0.5 * (f3 + f);
    vec2 w1 = 1.5 * f3 - 2.5 * f2 + 1.0;
    vec2 w2 = -1.5 * f3 + 2.0 * f2 + 0.5 * f;
    vec2 w3 = 0.5 * (f3 - f2);

    // Calculate the two weights and offsets for the 4 bilinear taps
    vec2 w12 = w1 + w2;
    vec2 offset12 = w2 / w12;

    // Define the 4 sampling points (bilinear fetches)
	vec2 texPos1  = p0 + 0.5;
    vec2 texPos0  = texPos1 - 1.0;
    vec2 texPos3  = texPos1 + 2.0;
    vec2 texPos12 = texPos1 + offset12;

    // Convert pixel coordinates back to normalized UV
    vec2 uv0  = texPos0  * srcTexelSize.zw;
    vec2 uv12 = texPos12 * srcTexelSize.zw;
    vec2 uv3  = texPos3  * srcTexelSize.zw;

    fragColor = 
        texture(inputTexture, vec2(uv0.x,  uv0.y))  * (w0.x * w0.y) +
        texture(inputTexture, vec2(uv12.x, uv0.y))  * (w12.x * w0.y) +
        texture(inputTexture, vec2(uv3.x,  uv0.y))  * (w3.x * w0.y) +
        texture(inputTexture, vec2(uv0.x,  uv12.y)) * (w0.x * w12.y) +
        texture(inputTexture, vec2(uv12.x, uv12.y)) * (w12.x * w12.y) +
        texture(inputTexture, vec2(uv3.x,  uv12.y)) * (w3.x * w12.y) +	
        texture(inputTexture, vec2(uv0.x,  uv3.y))  * (w0.x * w3.y) +
        texture(inputTexture, vec2(uv12.x, uv3.y))  * (w12.x * w3.y) +
        texture(inputTexture, vec2(uv3.x,  uv3.y))  * (w3.x * w3.y);
#endif
}
