#include "StaticMesh.hlsli"
#include "SceneConstants.hlsli"

float4 main(float4 position : POSITION) : SV_POSITION
{
	return mul(position,mul(world,camera.viewProjection));
}