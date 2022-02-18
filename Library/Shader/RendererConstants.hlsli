cbuffer RendererConstants : register(b3)
{
	float4 warmFactor; // w:intensity
	float4 coolFactor; // w:intensity

	float ambientIntensity = 0.25f;
	float specularIntensity;
	float specularPower;
	uint  halfLambert;

	float4 params;
	int4 flags;
};
