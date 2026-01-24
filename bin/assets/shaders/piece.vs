uniform vec4 transform;
uniform vec4 pieceMisc;
uniform vec4 tileSize;
uniform vec4 orthoMatrix;
out vec2 textureCoordinate;

vec2 rotate(vec2 v, vec2 r) {
	return vec2(dot(v,r), -v.x*r.y + v.y*r.x);
}
 
void main() {
	vec2 quad = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
	vec2 pos = transform.xy + rotate((quad - 0.5) *  tileSize.xy, transform.zw);
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
	gl_Position = vec4(pos, 0., 1.);
	textureCoordinate = quad; //inputPosition.xy;
}
