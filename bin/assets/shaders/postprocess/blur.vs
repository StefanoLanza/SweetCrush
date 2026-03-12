out     vec2 textureCoordinate;

void main() {
	vec2 tri = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	gl_Position.xy = -1.0 + 2.0 * tri; // to clip space
	textureCoordinate = tri;
}
