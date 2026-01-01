in      vec2 inputPosition;
uniform vec4 posRect;
out     vec2 textureCoordinate;

void main() {
	gl_Position = vec4(mix(posRect.xy, posRect.zw, inputPosition.xy * 2.0), 0., 1.);
	gl_Position.xy = -1.0 + 2.0 * gl_Position.xy; // to clip space
	textureCoordinate = inputPosition.xy * 2.0;
}
