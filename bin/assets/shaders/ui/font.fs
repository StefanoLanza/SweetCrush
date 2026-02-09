precision mediump float;

uniform sampler2D inputTexture;
uniform vec4 color;
uniform vec4 outlineColor;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
	// Single channel encoding of glyph and outline
//	float val = texture(inputTexture, textureCoordinate).r;
// 	float glyph = val > 0.5 ? 2.0*val - 1. : 0.;
//	float outline = (val > 0.5 ? 0. : 2.0*val) - glyph;
	// rgba encoding of glyph and outline
	vec4 val = texture(inputTexture, textureCoordinate);
 	float glyph = val.r;
	float outline = val.a;	
	fragColor.xyz = mix(outlineColor.xyz, color.xyz, glyph);
	fragColor.w = outline * color.a; // mix(outlineColor.a, color.a, glyph);
}
