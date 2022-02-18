#include "../Header/RendererConstatnts.h"


void RendererConstants::Initialize()
{
	rendererConstantBuffer = std::make_unique<StartingOver::ConstantBuffer<Constant>>();
	rendererConstantBuffer->data.colorFactor.warmColor = Vector4(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f,255.0f / 255.0f);
	rendererConstantBuffer->data.colorFactor.coolColor = Vector4(0.0f / 255.0f, 0.0f / 255.0f, 1.0f / 255.0f, 5.0f / 255.0f);

	rendererConstantBuffer->data.phongSharding.ambientIntensity = 0.25f;
	rendererConstantBuffer->data.phongSharding.specularIntensity = 0.20f;
	rendererConstantBuffer->data.phongSharding.specularPower = 90.0f;
	rendererConstantBuffer->data.halfLambert = 0;

}

void RendererConstants::Activate(const int slot, const bool vs, const bool ps)
{
	rendererConstantBuffer->Activate(slot, vs, ps);
}
