#include "Sprite.hlsli"
//--------------------------------------------
//	ピクセルシェーダー
//--------------------------------------------

float4 main(PSInput input) : SV_TARGET
{
	float4 colour = (float4)0;
	colour = DiffuseTexture.Sample(DecalSampler, input.Tex);
	colour *= input.Colour;
	return colour;
}