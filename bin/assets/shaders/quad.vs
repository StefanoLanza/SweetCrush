uniform   vec4 orthoMatrix;
out vec2 vTexCoord;

void main() {
	vec2 quad = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
	gl_Position = vec4(-1. + quad.x * orthoMatrix.x, 1. - quad.y * orthoMatrix.y, 0., 1.);
	vTexCoord = quad;
}
