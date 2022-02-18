#include "SkinnedMesh.hlsli"
#include "SceneConstants.hlsli"
#include "RendererConstants.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D ambientMap : register(t1);
Texture2D specularMap : register(t2);
Texture2D normalMap : register(t3);

SamplerState pointSamplerState : register(s0);
SamplerState linearSamplerState : register(s1);
SamplerState anisotropicSamplerState : register(s2);

float4 main(PSInput input) : SV_TARGET
{
	float4 diffuseMapColor = diffuseMap.Sample(anisotropicSamplerState,input.texCoord);
	float alpha = diffuseMapColor.a;

	float4 ambientMapColor = ambientMap.Sample(anisotropicSamplerState, input.texCoord) * ambientIntensity;

	float4 specularMapColor = specularMap.Sample(anisotropicSamplerState, input.texCoord);

	

	float3 N = normalize(input.normal.xyz);
	float3 T = normalize(input.tangent.xyz);
	float3 B = normalize(input.binormal.xyz);

	float3 L = normalize(-directionalLight.direction.xyz);

#if 0
	float diffuseFactor = max(dot(L, N), 0);
#else
	float diffuseFactor = max(0, dot(L, N)) * 0.5 + 0.5;
	diffuseFactor = diffuseFactor * diffuseFactor;
#endif

	float specularFactor = 0.0f;
	float3 E = normalize(camera.position.xyz - input.position.xyz);


	float3 fragmentColor = float3(0, 0, 0);

#if 0
	float3 coolColor = cool_factor.xyz + cool_factor.w * ambientMapColor.rgb;
	float3 warmColor = warm_factor.xyz + warm_factor.w * diffuseMapColor.rgb;
	float3 highlightColor = specularMapColor.rgb * float3(1, 1, 1);
	float3 litColor = lerp(warmColor, highlightColor, specularFactor);
	float3 unlitColor = 0.5 * coolColor;
	fragment_colour = unlitColor + diffuseFactor * directional_light.colour.xyz * directional_light.colour.w * litColor;
#else
	float3 Ka = ambientMapColor.rgb * (1 - diffuseFactor);
	float3 Kd = diffuseMapColor.rgb * diffuseFactor;
	float3 Ks = specularMapColor.rgb * specularFactor;
	fragmentColor = Ka + (Kd + Ks) * directionalLight.color.xyz * directionalLight.color.w;
#endif


	float3 color = float3(1, 1, 1);
	float range = 3;
	float3 p0[2];
	float3 p1[2];

	p0[0] = float3(0.58 + 0.1, 2.75 - 0.2, 6.18 - 0.1);
	p1[0] = float3(0.58 + 0.1, 2.75 - 0.2, 4.28 + 0.1);
	p0[1] = float3(1.12 - 0.1, 2.75 - 0.2, 6.18 - 0.1);
	p1[1] = float3(1.12 - 0.1, 2.75 - 0.2, 6.18 + 0.1);

	for(int i = 0;i < 2;i++)
	{
		float l = length(p1[i] - p0[i]);
		float3 d = normalize(p1[i] - p0[i]);
		float3 closestPoint = p0[i] + d * saturate(dot(input.position.xyz - p0[i], d) / l) * l;

		float3 pointLightDirection = closestPoint - input.position.xyz;
		float distance = length(pointLightDirection);
		float attenuation = 1 - saturate(distance * 1 / range);

		float3 L = normalize(pointLightDirection);

		diffuseFactor = dot(L, N) * attenuation * attenuation;

		fragmentColor.rgb += max(0, diffuseFactor) * color;

		float3 M = (p0[i] + p1[i]) * 0.5;
		L = normalize(M - input.position.xyz);
		float3 E = normalize(camera.position.xyz - input.position.xyz);

		float3 H = normalize(E + pointLightDirection);
		specularFactor = max(dot(H, N), 0);

		specularFactor = pow(specularFactor, 360) * 0.25;
		fragmentColor.rgb += specularFactor * color * specularMapColor.xyz * attenuation * attenuation;
	}
	return float4(fragmentColor, alpha);

}