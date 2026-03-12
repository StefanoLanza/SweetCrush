uniform   vec4 orthoMatrix;
uniform   vec4 coords; // center, radius, width
out vec4 radiusInterp;

void main() {
	vec2 quad = -1.0 + 2.0 * vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1)); // -1,1
	vec2 pos = coords.xy + coords.z * quad;
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
    gl_Position = vec4(pos, 0., 1.);
    radiusInterp = vec4(quad, 0.0, coords.z / coords.w);
}
