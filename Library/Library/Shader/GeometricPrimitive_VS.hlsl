#include "GeometricPrimitive.hlsli"

//VS_OUT main(float4 position : POSITION, float4 normal : NORMAL)
//{
//	VS_OUT vout;
//	vout.position = mul(position, WVP);
//
//	normal.w = 0;
//
//	float4 N = normalize(mul(normal, world));
//	float4 L = normalize(-light_Dir);
//
//	vout.colour = material_Colour * max(0, dot(L, N))/* + float4 (0.1,0.01,0.1,0.1)*/;
//	vout.colour.a = material_Colour.a;
//
//	return vout;
//
//}

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;
	float4 P = float4(input.Position, 1.0);

	output.Position = mul(matWVP, P);

	float3 wPos = mul(World, P).xyz;
	output.wPosition = wPos;

	float3 wN = mul((float3x3)World, input.Normal).xyz;
	wN = normalize(wN);
	output.wNormal = wN;

	output.Tex = input.Tex;
	output.Colour = float4(1, 1, 1, 1);

	output.vShadow = GetShadowTex(LightViewProjection, wPos.xyz);

	return output;
}

