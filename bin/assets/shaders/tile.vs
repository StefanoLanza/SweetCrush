in vec2 inputPosition;
uniform   vec4 orthoMatrix;
uniform   vec4 coords;
out vec2 textureCoordinate;
 
void main() {
	vec2 pos = coords.xy + coords.zw * inputPosition.xy;
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
	gl_Position = vec4(pos, 0., 1.);
	textureCoordinate = inputPosition.xy;
}
