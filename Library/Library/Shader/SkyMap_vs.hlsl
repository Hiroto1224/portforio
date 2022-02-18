#include "SkyMap.hlsli"
#include "SceneConstants.hlsli"

PSInput main(VSInput input)
{
	PSInput output;
	output.position = mul(input.position, mul(world, camera.viewProjection));
	output.texCoord = input.position.xyz;

	return output;
}
