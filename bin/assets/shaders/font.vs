precision highp float;

layout (location = 0) in vec2 inputPosition; // [0,1]
// Per-instance attributes
layout (location = 1) in vec4 posRect;
layout (location = 2) in vec4 uvRect;

uniform vec4 orthoMatrix;
out vec2 textureCoordinate;

void main() {
	vec2 pos = posRect.xy + posRect.zw * inputPosition.xy;
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
    gl_Position = vec4(pos, 0., 1.);
    textureCoordinate = uvRect.xy + uvRect.zw * inputPosition.xy;
}
