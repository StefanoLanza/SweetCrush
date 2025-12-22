precision highp float;

layout (location = 0) in vec2 inputPosition; 
// Per-instance attributes
layout (location = 3) in vec4 posRect; // FIXME 3 or another binding point ?
layout (location = 4) in vec4 uvRect;

uniform vec4 orthoMatrix;
out vec2 textureCoordinate;

void main() {
	vec2 pos = mix(posRect.xy, posRect.zw, inputPosition.xy);
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
    gl_Position = vec4(pos, 0., 1.);
    textureCoordinate = mix(uvRect.xy, uvRect.zw, inputPosition.xy);
}
