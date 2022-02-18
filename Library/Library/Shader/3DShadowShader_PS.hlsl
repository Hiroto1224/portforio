#include "3DShadowShader.hlsli"

cbuffer CBColour : register(b2)
{
	float4 materialColour;
};

Texture2D shadowMap : register(t0);
Texture2D diffuseMap : register(t1);

Texture2D normalMap : register(t3);

SamplerComparisonState shadowMapSamplerState : register(s0);
SamplerState diffuseMapSamplerState : register(s1);
SamplerState normalMapSamplerState : register(s2);


float4 PSMain(VS_OUT pin) : SV_TARGET
{
	float4 outColour;

	float4 mapDiff = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * materialColour;

	float4 normalMapColour = normalMap.Sample(normalMapSamplerState, pin.texcoord);
	normalMapColour = (normalMapColour * 2.0f) - 1.0f;
	normalMapColour.w = 0;

	float3 N = normalize(pin.normal.xyz);
	float3 T = normalize(pin.tangent.xyz);
	float3 B = normalize(cross(N, T));
	N = normalize((normalMapColour.x * T) + (normalMapColour.y * B) + (normalMapColour.z * N));

	float3 L = normalize(-lightDir.xyz);

	float diffuseFactor = saturate(dot(N, L));
	diffuseFactor = diffuseFactor * diffuseFactor;

	float3 shadowCoord = pin.shadowCoord.xyz / pin.shadowCoord.w;

	float maxDepthSlope = max(abs(ddx(shadowCoord.z)), abs(ddy(shadowCoord.z)));
	float shadowThreshold = 1.0f;
	float slopeScaleBias = 0.0001f;
	float depthBiasClamp = 0.01f;
	float3 shadowColour = float3(0.65f, 0.65f, 0.65f);

	float shadowBias = bias + slopeScaleBias * maxDepthSlope;
	shadowBias = min(shadowBias, depthBiasClamp);
	shadowThreshold = shadowMap.SampleCmpLevelZero(shadowMapSamplerState, shadowCoord.xy, shadowCoord.z - shadowBias);
	shadowColour = lerp(shadowColour, float3(1.0f, 1.0f, 1.0f), shadowThreshold);

	float3 Ka = mapDiff.rgb * (1 - diffuseFactor) * 0.8f;
	float3 Kb = mapDiff.rgb * diffuseFactor * 1.5f;
	outColour.rgb = (Ka + Kb) * shadowColour;
	outColour.a = mapDiff.a;
	//outColour = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return outColour;
}