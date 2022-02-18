#include "DepthShadowMap.hlsli"

//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
PSInputShadow main(VSInput input)
{
	PSInputShadow output = (PSInputShadow)0;
	float4 P = float4(input.Position, 1.0);
	// WVP変換.
	output.Position = mul(matWVP, P);

	return output;
}
