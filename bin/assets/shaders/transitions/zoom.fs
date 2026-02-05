precision mediump float;

layout(binding = 0)  uniform sampler2D texture0; // new
layout(binding = 1)  uniform sampler2D texture1; // snapshot
in vec2 vTexCoord;
uniform vec4 misc; // dir, progress, zoom
out vec4 fragColor;

void main() {
	vec2 uv0 = 0.5 + (vTexCoord - 0.5) * mix(misc.z, 1.0, misc.y); // zoom in
	vec2 uv1 = 0.5 + (vTexCoord - 0.5) * mix(1.0, misc.z, misc.y); // zoom out
	float f = mix(1.0, 0.0, min(1.0, misc.y)); // fade out current
	float f2 = 1.0 - f; //mix(0.0, 1.0, min(1.0, misc.y * 8.0)); // fade in current
	//float l = float( (abs(uv1.x - 0.5) > 0.5) || (abs(uv1.y - 0.5) > 0.5) ) ;
    vec4 tex0 = texture(texture0, uv0) * f2;
    vec4 tex1 = texture(texture1, uv1) * f;
	//tex0 = mix(tex0, color0, abs(uv0.x - 0.5) > 0.5); // make out of bounds black
    fragColor =  tex0 + tex1; //mix(tex1, tex0, l);
}
