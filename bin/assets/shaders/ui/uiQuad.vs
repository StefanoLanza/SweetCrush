uniform   vec4 orthoMatrix;
highp uniform   vec4 posRect;
out vec2 textureCoordinate;
 
void main() {
	vec2 quad = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
	vec2 pos = posRect.xy + posRect.zw * quad;
    gl_Position = vec4(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y, 0., 1.);
    textureCoordinate = quad;
}
