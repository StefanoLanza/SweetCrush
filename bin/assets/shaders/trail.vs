uniform   vec4 orthoMatrix;
uniform   vec4 coords;
uniform vec4 width;
out vec2 textureCoordinate;

void main() {
	vec2 quad = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
	vec2 dir = normalize(coords.zw - coords.xy);
	vec2 perp = vec2(-dir.y, dir.x);
	vec2 pos = mix(coords.xy, coords.zw, quad.x);
	pos += perp * width.x * (quad.y - 0.5);
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
    gl_Position = vec4(pos, 0., 1.);
    textureCoordinate = quad;
}
