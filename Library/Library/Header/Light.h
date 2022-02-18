#pragma once
#include "MyMath.h"
#include "ConstantBuffer.h"

#include <d3d11.h>
#include <wrl.h>

struct SpotLight {
	float index;
	float range;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type; //—LŒø‚©–³Œø‚©
	float innerCorn;
	float outerCorn;
	float dummy0;
	float dummy1;
	float dummy2;
	Vector4 pos;
	Vector4 color;
	Vector4 dir;
};

struct PointLight {
	float index;
	float range;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type; //—LŒø‚©–³Œø‚©
	float dummy;
	Vector4 pos;
	Vector4 color;
};

struct DirectionalLight
{
	Vector4 direction;
	Vector4 color;
};

class Light
{
public:

	Light() = default;
	virtual ~Light() = default;
	Light(Light&) = delete;
	Light& operator=(Light&) = delete;

	void Initialize();

	void SetDirectionalLight(Vector3 dir,Vector4 color);

	void SetDirectionalLightDirection(Vector3 dir);

	void SetAmbient(Vector4 ambient);

	void SetPointLight(int index, Vector3 pos, Vector4 color, float range);

	void SetSpotLight(int index, Vector3 pos, Vector4 color, Vector3 dir, float range, float inner, float outer);

	Vector4 GetDirectionalLightDirection() { return lightData.directionalLight.direction; }
	
	void Activate(const int slot,const bool vs,const bool ps);

private:

	static constexpr int PointMax = 32;
	static constexpr int SpotMax = 32;

	struct LightData
	{
		Vector4 ambient;
		PointLight pointLight[PointMax];
		SpotLight spotLight[SpotMax];
		DirectionalLight directionalLight;
	};

	LightData lightData;

	std::unique_ptr<StartingOver::ConstantBuffer<LightData>> lightConstant;


};


