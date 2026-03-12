// Per-instance attributes
layout (location = 0) in vec4 tileCoords;
layout (location = 1) in vec4 tileColor;

out vec2 textureCoordinate;
out vec4 color;
 
uniform vec4 orthoMatrix;

void main() {
	vec2 quad = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
	vec2 pos = tileCoords.xy + tileCoords.zw * quad;
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
	gl_Position = vec4(pos, 0., 1.);
	textureCoordinate = quad;
	color = tileColor;
}
