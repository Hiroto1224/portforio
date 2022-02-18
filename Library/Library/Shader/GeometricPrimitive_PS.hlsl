#include "geometricPrimitive.hlsli"

//float4 main(VS_OUT pin) : SV_TARGET
//{
//	return pin.colour;
//}

//--------------------------------------------
//	�s�N�Z���V�F�[�_�[
//--------------------------------------------
float4 main(PSInput input) : SV_Target
{
	float4 colour = DiffuseTexture.Sample(DecalSampler,input.Tex);
	float3 N = normalize(input.wNormal);
	float3 E = normalize(EyePos.xyz - input.wPosition);
	float3 L = normalize(LightDir.xyz);
	// ����
	float3 A = AmbientColor.rgb;

	//�g�U����
	float3 C = LightColor.rgb;
	float3 Kd = float3(1.0f, 1.0f, 1.0f);
	float3 D = Diffuse(N, L, C, Kd);

	//���ʔ���
	float3 Ks = float3(1.0f, 1.0f, 1.0f);
	float3 S = PhongSpecular(N, L, C, E, Ks, 20);

	colour *= input.Colour * float4(A + D + S, 1.0);


	// �V���h�E�}�b�v�K�p
	colour.rgb *= GetShadow(ShadowTexture, ShadowSampler, input.vShadow, ShadowColor, Bias);

	return colour;
}

