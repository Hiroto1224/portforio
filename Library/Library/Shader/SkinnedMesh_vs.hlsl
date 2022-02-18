#include "SkinnedMesh.hlsli"
#include "SceneConstants.hlsli"

PSInput main(VSInput input)
{
	float3 blendedPosition = { 0,0,0 };
	float3 blendedNormal = { 0,0,0 };
	for(int i = 0; i < 4; i++)
	{
		blendedPosition += (input.boneWeights[i] * mul(input.position, boneTransforms[input.boneIndices[i]])).xyz;
		blendedNormal += (input.boneWeights[i] * mul(float4(input.normal.xyz,0), boneTransforms[input.boneIndices[i]]));
	}
	input.position = float4(blendedPosition, 1.0f);
	input.normal = float4(blendedNormal, 0.0f);

	PSInput output;
	output.svPosition = mul(input.position, mul(world, camera.viewProjection));

	output.position = mul(input.position, world);

	input.normal.w = 0;
	output.normal = normalize(mul(input.normal, world));
	
	float sigma = input.tangent.w;
	input.tangent.w = 0;
	output.tangent = normalize(mul(input.tangent, world));

	output.binormal.xyz = cross(input.normal.xyz, input.tangent.xyz) * sigma;
	output.binormal.w = 0;

	output.texCoord = input.texCoord;

	return output;

}