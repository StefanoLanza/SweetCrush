in vec2 inputPosition;
uniform   vec4 orthoMatrix;
uniform   vec4 coords;
uniform   vec4 perp;
out vec2 textureCoordinate;

void main() {
	vec2 pos = mix(coords.xy, coords.zw, inputPosition.x);
	pos += perp.xy * (inputPosition.y - 0.5);
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
    gl_Position = vec4(pos, 0., 1.);
    textureCoordinate = inputPosition.xy;
}
