#include "Rasterizer.hlsli"

float4 main(PS_INPUT psInput) : SV_TARGET
{
	return textureMap.Sample(textureMapSamplerState,psInput.texCoord) * psInput.color;
}


