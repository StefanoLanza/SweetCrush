#version 300 es

precision mediump float;

uniform sampler2D inputTexture;
uniform vec4 color;
uniform vec4 outlineColor;
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
//	float val = texture(inputTexture, textureCoordinate).a;
// 	float glyph = val > 0.5 ? 2*val - 1. : 0.;
//	float outline = (val > 0.5 ? 1. : 2*val) - glyph;
	vec4 val = texture(inputTexture, textureCoordinate);
 	float glyph = val.r;
	float outline = val.a - glyph;
	
	vec3 glyphColor = color.xyz;
	float glyphAlpha = glyph * color.w;
	float outlineAlpha = outline * outlineColor.a;

	vec3 outColor;	
	float alpha;
	// Simulate blend( blend (background, glyph), outline):
	// outColor.rgb = b.rgb * (1 - g.a) * (1 - o.a) + g.rgb * g.a * (1 - o.a) + o.rgb * o.a
	alpha = glyphAlpha + outlineAlpha - glyphAlpha * outlineAlpha;

	fragColor.xyz = mix(glyphColor * glyphAlpha, vec3(1.,1.,1.) * outlineColor.rgb, outlineAlpha);
	fragColor.w = alpha;
}
