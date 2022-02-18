#include "SceneConstants.hlsli"
#include "StaticMesh.hlsli"

PSInput main(VSInput vin)
{
	PSInput vout;
	vout.sv_position = mul(vin.position, mul(world, camera.viewProjection));

	vout.position = mul(vin.position, world);

	vin.normal.w = 0;
	vout.normal = normalize(mul(vin.normal, world));
	//vout.normal = vin.normal ;
	float sigma = vin.tangent.w;
	vin.tangent.w = 0;
	vout.tangent = normalize(mul(vin.tangent, world));

	vout.binormal.xyz = cross(vin.normal.xyz, vin.tangent.xyz) * sigma;
	vout.binormal.w = 0;

	vout.texcoord = vin.texcoord;

	return vout;
}

