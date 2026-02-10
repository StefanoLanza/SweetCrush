precision mediump float;

uniform sampler2D inputTexture;
highp uniform   vec4 coords;
uniform   vec4 _9Patch; // margin, margin / bitmapWidth, margin / bitmapHeight, grayScale
uniform   vec4 uvRect;
uniform vec4 color;
in vec2 textureCoordinate;
out vec4 fragColor; 

// 9-patch adjustment
vec2 compute_uv(vec2 xy) {
	vec2 xy0 = vec2(0.0);
	vec2 xy1 = _9Patch.xx;
	vec2 xy2 = coords.zw - _9Patch.xx;
	vec2 uv = vec2(0.0);
	if (_9Patch.x > 0.0) {
		uv += clamp((xy - xy0) / _9Patch.x, 0.0, 1.0) * _9Patch.yz;
		uv += clamp((xy - xy1) / (coords.zw - 2.0 * _9Patch.x), 0.0, 1.0) * (1.0 - 2.0 * _9Patch.yz);
		uv += clamp((xy - xy2) / (_9Patch.x), 0.0, 1.0) * _9Patch.yz;
	}
	else {
		uv = xy / coords.zw;
	}
	return uv;
}

void main() {
	vec2 uv01 = compute_uv(coords.zw * textureCoordinate);    // 0, 1 
	vec2 atlasUV = mix(uvRect.xy, uvRect.zw, uv01); // to atlas UVs
    vec4 tex = color * texture(inputTexture, atlasUV, 0.);
	float gray = dot(tex.rgb, vec3(0.299, 0.587, 0.114));
    fragColor.xyz = mix(tex.xyz, vec3(gray), _9Patch.w);
	fragColor.w = tex.w;
}
