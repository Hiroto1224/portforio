#include "SceneConstants.hlsli"
#include "SkinnedMesh.hlsli"


float4 main(float4 position : POSITION, float4 boneWeights : WEIGHTS, uint4 boneIndices : BONES) : SV_POSITION
{

	float3 blendedPosition = {0,0,0};
	for (int i = 0; i < 4; i++)
	{
		blendedPosition += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
	}

	position = float4(blendedPosition, 1.0f);

	return mul(position,mul(world,camera.viewProjection));
}