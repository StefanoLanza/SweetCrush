in vec2 inputPosition;
uniform   vec4 orthoMatrix;
uniform   vec4 posRect;
uniform   vec4 uvRect;
uniform   vec4 rotation;
out vec2 textureCoordinate;

vec2 rotate(vec2 v, vec2 r) {
	v -= rotation.zw;
	return vec2(dot(v,r), -v.x*r.y + v.y*r.x) + rotation.zw;
}
 
void main() {
	vec2 pos = posRect.xy + posRect.zw * inputPosition.xy;
	pos = rotate(pos, rotation.xy);
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
    gl_Position = vec4(pos, 0., 1.);
	textureCoordinate = (uvRect.xy + inputPosition.xy) * uvRect.zw; // atlas UVs
}
