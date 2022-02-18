#pragma once
#include <memory>
#include "MyMath.h"
#include "ConstantBuffer.h"

struct ColorFactor
{
	Vector4 warmColor;
	Vector4 coolColor;
};

struct PhongSharding
{
	float ambientIntensity;
	float specularIntensity;
	float specularPower;
};

class RendererConstants
{
public:

	RendererConstants() = default;
	virtual ~RendererConstants() = default;
	RendererConstants(RendererConstants&) = delete;
	RendererConstants& operator=(RendererConstants&) = delete;

	void Initialize();

	void Activate(const int slot, const bool vs, const bool ps);

private:

	struct Constant
	{
		ColorFactor colorFactor;
		PhongSharding phongSharding;
		std::uint32_t halfLambert;
		float params[4] = { 0,0,0,1 };
		int flags[4] = { 0,0,0,0 };
	};

	std::unique_ptr<StartingOver::ConstantBuffer<Constant>> rendererConstantBuffer;
};