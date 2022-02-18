
TextureCube cubeMap : register(t0);

SamplerState pointSamplerState : register(s0);
SamplerState linearSamplerState : register(s1);
SamplerState anisotropicSamplerState : register(s2);



struct VSInput
{
	float4 position : POSITION;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float3 texCoord : TEXCOORD;
};


cbuffer CONSTANTBUFFER :register(b0)
{
	column_major  float4x4 world;
	float4 color;
}