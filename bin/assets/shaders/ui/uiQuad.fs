precision mediump float;

uniform sampler2D inputTexture;
highp uniform   vec4 posRect;
uniform   vec4 _9Patch; // margin, margin / bitmapWidth, margin / bitmapHeight
uniform   vec4 uvRect;
uniform vec4 color;
in vec2 textureCoordinate;
out vec4 fragColor; 

// 9-patch adjustment
vec2 compute_uv(vec2 xy) {
	vec2 xy0 = vec2(0.0);
	vec2 xy1 = _9Patch.xx;
	vec2 xy2 = posRect.zw - _9Patch.xx;
	vec2 uv = vec2(0.0);
	if (_9Patch.x > 0.0) {
		uv += clamp((xy - xy0) / _9Patch.x, 0.0, 1.0) * _9Patch.yz;
		uv += clamp((xy - xy1) / (posRect.zw - 2.0 * _9Patch.x), 0.0, 1.0) * (1.0 - 2.0 * _9Patch.yz);
		uv += clamp((xy - xy2) / (_9Patch.x), 0.0, 1.0) * _9Patch.yz;
	}
	else {
		uv = xy / posRect.zw;
	}
	return uv;
}

void main() {
	vec2 uv01 = compute_uv(posRect.zw * textureCoordinate);    // 0, 1 
	vec2 atlasUV = mix(uvRect.xy, uvRect.zw, uv01); // to atlas UVs
    fragColor = color * texture(inputTexture, atlasUV, 0.);
}
