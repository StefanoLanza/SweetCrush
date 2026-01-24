precision mediump float;

uniform sampler2D inputTexture;
uniform vec4 color;
in vec4 radiusInterp;
out vec4 fragColor; 

void main() {
	float r = length(radiusInterp.xy);
	float v = clamp((1.0 - r) * radiusInterp.w, 0.0, 1.0);
	fragColor = color * texture(inputTexture, vec2(0.5, v), 0.);  // additive
}
