in vec2 inputPosition;
uniform vec4 transform;
uniform vec4 pieceMisc;
uniform vec4 tileSize;
uniform vec4 orthoMatrix;
out vec2 textureCoordinate;

vec2 rotate(vec2 v, vec2 r) {
	return vec2(dot(v,r), -v.x*r.y + v.y*r.x);
}
 
void main() {
	vec2 pos = transform.xy + rotate(inputPosition.xy - 0.5, transform.zw) * tileSize.xy;
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
	gl_Position = vec4(pos, 0., 1.);
	textureCoordinate = inputPosition.xy;
}
