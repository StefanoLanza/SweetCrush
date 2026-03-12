uniform   vec4 orthoMatrix;
highp uniform   vec4 coords;
uniform   vec4 thickness;
 
void main() {
	vec2 diff = coords.zw - coords.xy;
	vec2 perp = normalize(vec2(-diff.y, diff.x));
	vec2 quad = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
	vec2 pos = mix(coords.xy, coords.zw, quad.x) + perp * (quad.y - 0.5) * thickness.x * 2.0;
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
	gl_Position = vec4(pos, 0., 1.);
}
