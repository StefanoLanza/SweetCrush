#version 300 es

in      vec2 inputPosition;
uniform vec4 posRect;
uniform vec4 uvRect;
out     vec2 textureCoordinate;

void main() {
	gl_Position = vec4(mix(posRect.xy, posRect.zw, inputPosition.xy), 0., 1.);
    textureCoordinate = mix(uvRect.xy, uvRect.zw, inputPosition.xy);
}
