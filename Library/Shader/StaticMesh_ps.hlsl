#include "StaticMesh.hlsli"
#include "SceneConstants.hlsli"
#include "RendererConstants.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D ambientMap : register(t1);
Texture2D specularMap : register(t2);
Texture2D normalMap : register(t3);

SamplerState pointSamplerState : register(s0);
SamplerState linearSamplerState : register(s1);
SamplerState anisotropicSamplerState : register(s2);

float4 main(PSInput pin) : SV_TARGET
{
	float4 diffuseMapColor = diffuseMap.Sample(anisotropicSamplerState, pin.texcoord) * color;
	float alpha = diffuseMapColor.a;

	float ambientFactor = ambientIntensity;
	float4 ambientMapColor = ambientMap.Sample(anisotropicSamplerState, pin.texcoord) * ambientIntensity;

	float4 specularMapColor = specularMap.Sample(anisotropicSamplerState, pin.texcoord);

	float4 normalMapColor = normalMap.Sample(pointSamplerState, pin.texcoord);
	normalMapColor = normalMapColor - 0.5f;
	normalMapColor.w = 0;

	// transform to world space from tangent space
	//                 |Tx Ty Tz|
	// normal = |x y z||Bx By Bz|
	//                 |Nx Ny Nz|
	float3 N = normalize(pin.normal.xyz);
//	return float4(N, 1);
	float3 T = normalize(pin.tangent.xyz);
	//T = normalize(T - dot(N, T));
	float3 B = normalize(pin.binormal.xyz);
	//B = normalize(T - dot(N, B));
	N = normalize((normalMapColor.x * T) + (normalMapColor.y * B) + (normalMapColor.z * N));

	float3 L = normalize(-directionalLight.direction.xyz);
	float diffuseFactor = max(0, dot(L, N)) * 0.5 + 0.5;
	diffuseFactor = diffuseFactor * diffuseFactor;

	float specularFactor = 0.0;
	float3 E = normalize(camera.position.xyz - pin.position.xyz);


	float3 fragmentColor = float3(0, 0, 0);
	//directional light
#if 0
	float3 cool_colour = cool_factor.xyz + cool_factor.w * ambientMapColor.rgb * 3;
	float3 warm_colour = warm_factor.xyz + warm_factor.w * diffuseMapColor.rgb;
	float3 highlight_colour = specularMapColor.rgb * float3(2, 2, 2);
	float3 lit_colour = lerp(warm_colour, highlight_colour, specularFactor);
	float3 unlit_colour = cool_colour;
	fragmentColor = unlit_colour;
	fragmentColor += diffuseFactor * directional_light.colour.xyz * directional_light.colour.w * lit_colour;
#else
	float3 Ka = ambientMapColor.rgb * (1 - diffuseFactor);
	float3 Kd = diffuseMapColor.rgb * diffuseFactor;
	float3 Ks = specularMapColor.rgb * specularFactor;
	fragmentColor = (Ka + Kd + Ks) * directionalLight.color.xyz * directionalLight.color.w;
#endif

	//point light
	float3 colour = float3(1, 1, 1);
	float range = 3;
	float3 p0[2];
	float3 p1[2];
	//p0[0] = float3(0.85, 2.75 - 0.2, 6.18);
	//p1[0] = float3(0.85, 2.75 - 0.2, 4.28);
	p0[0] = float3(0.58 + 0.1, 2.75 - 0.2, 6.18 - 0.1);
	p1[0] = float3(0.58 + 0.1, 2.75 - 0.2, 4.28 + 0.1);
	p0[1] = float3(1.12 - 0.1, 2.75 - 0.2, 6.18 - 0.1);
	p1[1] = float3(1.12 - 0.1, 2.75 - 0.2, 4.28 + 0.1);

	for (int i = 0; i < 2; i++)
	{
		float l = length(p1[i] - p0[i]);
		float3 d = normalize(p1[i] - p0[i]);
		float3 closestPoint = p0[i] + d * saturate(dot(pin.position.xyz - p0[i], d) / l) * l;

		float3 pointLightDirection = closestPoint - pin.position.xyz;
		float distance = length(pointLightDirection);
		float attenuation = 1 - saturate(distance * 1 / range);

		float3 L = normalize(pointLightDirection);

		diffuseFactor = dot(L, N) * attenuation * attenuation * 0.5;

		fragmentColor.rgb += max(0, diffuseFactor) * colour;
		float3 M = (p0[i] + p1[i]) * 0.5;
		L = normalize(M - pin.position.xyz);
		float3 E = normalize(camera.position.xyz - pin.position.xyz);

		float3 H = normalize(E + pointLightDirection);
		specularFactor = max(dot(H, N), 0);

		specularFactor = pow(specularFactor, 360) * 0.25;
#if 1
		//float specularFactor = 0.0;
		//float3 E = normalize(camera.position.xyz - pin.position.xyz);

		//float3 H = normalize(E + L);
		//specularFactor = max(dot(H, N), 0);
		////float3 R = normalize(reflect(-L, N));
		////specularFactor = max(dot(R, E), 0);


		//specularFactor = pow(specularFactor, 180);
		//specularFactor = pow(specularFactor, specular_power) * specular_intensity;
#else
		//float3 M = (p0[i] + p1[i]) * 0.5;
		//L = normalize(M - pin.position.xyz);
		//float3 E = normalize(camera.position.xyz - pin.position.xyz);

		//float3 H = normalize(E + L);
		//specularFactor = max(dot(H, N), 0);
		////float3 R = normalize(reflect(-L, N));
		////specularFactor = max(dot(R, E), 0);

		//specularFactor = pow(specularFactor, 180);
#endif
		fragmentColor.rgb += specularFactor * colour * specularMapColor.xyz/* * attenuation*/ * attenuation * 1.0;
	}
	
	//float3 pointLightDirection = point_light.position.xyz - pin.position.xyz;
	//float distance = length(pointLightDirection);
	//float reciprocal_range = point_light.colour.w;
	//float attenuation = 1 - saturate(distance * reciprocal_range);
	//fragmentColor.rgb += max(0, dot(normalize(pointLightDirection), N) * attenuation * attenuation) * point_light.colour.rgb;

	return float4(fragmentColor, alpha);
}
