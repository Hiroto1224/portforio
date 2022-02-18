
#include "Font.hlsli"

VS_OUT vsMain(
	float4 position : POSITION,
	float2 texcoord : TEXCOORD,
	float4 color : COLOUR

)
{
	VS_OUT vout;
	vout.position = position;
	vout.color = color;
	vout.texcoord = texcoord;

	return vout;
}
