precision mediump float;

uniform sampler2D inputTexture;
uniform vec4 color;
uniform vec4 misc; // grayscale, 
in vec2 textureCoordinate;
out vec4 fragColor; 

void main() {
	vec4 tex = color * texture(inputTexture, textureCoordinate); 
	float gray = dot(tex.rgb, vec3(0.299, 0.587, 0.114));
    fragColor.xyz = mix(tex.xyz, vec3(gray), misc.x);
	fragColor.w = tex.w;
}
