#include "SkyMap.hlsli"
#include "SceneConstants.hlsli"

float4 main(PSInput input) : SV_TARGET
{

	return float4(cubeMap.Sample(anisotropicSamplerState,input.texCoord));
}