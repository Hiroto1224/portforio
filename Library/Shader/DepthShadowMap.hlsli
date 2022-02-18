//--------------------------------------------
//	テクスチャ
//--------------------------------------------
Texture2D DiffuseTexture : register(t0);
SamplerState DecalSampler : register(s0);


//--------------------------------------------
//	グローバル変数
//--------------------------------------------
cbuffer CBPerMesh : register(b0)
{
	matrix  World;
	matrix	matWVP;
};



//--------------------------------------------
//	データーフォーマット
//--------------------------------------------
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal   : NORMAL;
	float2 Tex      : TEXCOORD;
	float4 Color    : COLOUR;	//頂点カラー無し
};



//バーテックスシェーダー出力構造体
struct PSInputShadow
{
	float4 Position : SV_POSITION;
};


