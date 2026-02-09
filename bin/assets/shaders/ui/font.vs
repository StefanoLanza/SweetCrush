precision highp float;

// Per-instance attributes
layout (location = 0) in vec4 coords;
layout (location = 1) in vec4 uvRect;

uniform vec4 orthoMatrix;
out vec2 textureCoordinate;

void main() {
	vec2 quad = vec2(float(gl_VertexID & 1), float((gl_VertexID >> 1) & 1));
	vec2 pos = coords.xy + coords.zw * quad;
    gl_Position = vec4(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y, 0., 1.);
    textureCoordinate = uvRect.xy + uvRect.zw * quad;
}
