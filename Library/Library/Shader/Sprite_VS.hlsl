//--------------------------------------------
//	頂点シェーダー
//--------------------------------------------
#include "Sprite.hlsli"

PSInput main(VSInput input)
{
	PSInput output = (PSInput)0;

	// 出力値設定.
	output.Position = float4(input.Position, 1);
	output.Colour = input.Colour;
	output.Tex = input.Tex;

	return output;
}
