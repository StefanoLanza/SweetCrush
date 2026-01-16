in vec2 inputPosition;
uniform   vec4 orthoMatrix;

// Per-instance attributes
layout (location = 1) in vec4 pieceCoords;
layout (location = 2) in vec4 pieceMisc;
layout (location = 3) in vec4 pieceColor;

uniform vec4 tileSize;
//uniform   vec4 rotation;
out vec4 color;
out vec2 textureCoordinate;

#if 0
vec2 rotate(vec2 v, vec2 r) {
	vec2 pivot = 0.5 * (coords.xy + coords.zw * 0.5);
	v -= pivot;
	return vec2(dot(v,r), -v.x*r.y + v.y*r.x) + pivot;
}
#endif
 
void main() {
	vec2 scaledTileSize = tileSize.xy * pieceMisc.y;
	vec2 pos = pieceCoords.xy + (tileSize.xy - scaledTileSize) * 0.5 + scaledTileSize * inputPosition.xy;
	//pos = rotate(pos, rotation.xy);
	pos = vec2(-1. + pos.x * orthoMatrix.x, 1. - pos.y * orthoMatrix.y);
	gl_Position = vec4(pos, 0., 1.);
	int id = int(pieceMisc.x);
	textureCoordinate = (inputPosition.xy + vec2(float(id % 4), float(id / 4))) * tileSize.xy / vec2(256.0);
	color = pieceColor;
}
