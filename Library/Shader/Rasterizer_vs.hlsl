#include "Rasterizer.hlsli"



PS_INPUT main(VSInput input)
{
	PS_INPUT psInput;
	psInput.position = input.position;
	psInput.texCoord = input.texCoord;
	psInput.color = input.color;
	return psInput;
}
