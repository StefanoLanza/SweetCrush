in vec2 inputPosition;
uniform   vec4 orthoMatrix;
uniform   vec4 posRect;
uniform   vec4 uvRect;
uniform   vec4 rotation;
uniform   vec4 _9Patch; // margin, margin / bitmapWidth, margin / bitmapHeight
out vec2 textureCoordinate;

vec2 rotate(vec2 v, vec2 r) {
	v -= rotation.zw;
	return vec2(dot(v,r), -v.x*r.y + v.y*r.x) + rotation.zw;
}

// 9-patch adjustment
vec2 compute_uv(vec2 xy) {
	float eps = 1e-5;
	vec2 xy0 = vec2(0.0);
	vec2 xy1 = _9Patch.xx;
	vec2 xy2 = posRect.zw - _9Patch.xx;
	vec2 uv = vec2(0.0);
	uv += clamp((xy - xy0) / (_9Patch.x + eps), 0.0, 1.0) * _9Patch.yz;
	uv += clamp((xy - xy1) / (posRect.zw - 2.0 * _9Patch.x), 0.0, 1.0) * (1.0 - 2.0 * _9Patch.yz);
	uv += clamp((xy - xy2) / (_9Patch.x + eps), 0.0, 1.0) * _9Patch.yz;
	return uv;
}
 
void main() {
	vec2 pos = posRect.xy + posRect.zw * inputPosition.xy;
	pos = rotate(pos, rotation.xy);
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
    gl_Position = vec4(pos, 0., 1.);
    textureCoordinate = compute_uv(posRect.zw * inputPosition.xy);    // 0, 1 UVs
	textureCoordinate = mix(uvRect.xy, uvRect.zw, textureCoordinate); // to atlas UVs
}
