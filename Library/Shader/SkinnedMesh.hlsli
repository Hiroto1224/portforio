struct VSInput
{
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float2 texCoord : TEXCOORD;
	float4 boneWeights : WEIGHTS;
	uint4 boneIndices : BONES;
};


struct PSInput
{
	float4 svPosition : SV_POSITION;
	float4 position : POSITION;
	float4 normal : NORMAL;
	float4 tangent : TANGENT;
	float4 binormal : BINORMAL;
	float2 texCoord : TEXCOORD;
};

#define MAXBONES 128
cbuffer OBJECTCONSTANTS : register(b0)
{
	column_major float4x4 world;
	column_major float4x4 boneTransforms[MAXBONES];
	float4 color;

}
