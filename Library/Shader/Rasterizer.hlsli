Texture2D textureMap : register(t0);
SamplerState textureMapSamplerState : register(s0);
Texture2DMS<float4> msTextureMap : register(t1);

struct VSInput
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD;
	float4 color    : COLOR;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
};

