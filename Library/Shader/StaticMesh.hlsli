struct VSInput
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texcoord : TEXCOORD;
};

struct PSInput
{
	float4 sv_position : SV_POSITION;
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float2 texcoord : TEXCOORD;
};

cbuffer OBJECT_CONSTANTS : register(b0)
{
	column_major float4x4 world;
	float4 color;
};

