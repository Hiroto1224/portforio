#include "Diagram.hlsli"

PSInput VSMain(VSInput _input)
{
	PSInput output = (PSInput)0;
	output.position = float4(_input.position, 1);
	output.color = _input.color;
	
	return output;
}