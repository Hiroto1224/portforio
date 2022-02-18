#include "Rasterizer.hlsli"


float4 main(PS_INPUT psInput, uint sampleIndex : SV_SAMPLEINDEX) : SV_TARGET
{
	uint width,height,samplerNum;
	msTextureMap.GetDimensions(width, height, samplerNum);
	return msTextureMap.Load(uint2(psInput.texCoord.x * width, psInput.texCoord.y * height), sampleIndex) * psInput.color;
}