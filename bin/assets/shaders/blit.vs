uniform vec4 posRect;
out     vec2 textureCoordinate;

void main() {
	vec2 tri = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
	gl_Position = vec4(mix(posRect.xy, posRect.zw, tri), 0., 1.);
	gl_Position.xy = -1.0 + 2.0 * gl_Position.xy; // to clip space
	textureCoordinate = tri;
}
