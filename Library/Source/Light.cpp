#include "../Header/Light.h"
#include <d3d11.h>


void Light::Initialize()
{
	lightConstant = std::make_unique<StartingOver::ConstantBuffer<LightData>>();

	ZeroMemory(lightData.pointLight, sizeof(PointLight) * PointMax);
	ZeroMemory(lightData.spotLight, sizeof(SpotLight) * SpotMax);
	ZeroMemory(lightConstant->data.pointLight, sizeof(PointLight) * PointMax);
	ZeroMemory(lightConstant->data.spotLight, sizeof(SpotLight) * SpotMax);

}


void Light::SetDirectionalLight(Vector3 dir, Vector4 color)
{
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	if (d > 0) { dir.x /= d; dir.y /= d; dir.z /= d; }
	lightData.directionalLight.direction = Vector4(dir.x, dir.y, dir.z, 0);
	lightData.directionalLight.color = Vector4(color.x, color.y, color.z, color.w);

}


void Light::SetDirectionalLightDirection(Vector3 dir)
{
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	if (d > 0) { dir.x /= d; dir.y /= d; dir.z /= d; }
	lightData.directionalLight.direction = Vector4(dir.x, dir.y, dir.z, 0);

}

void Light::SetAmbient(Vector4 amb)
{
	lightData.ambient = Vector4(amb.x, amb.y, amb.z, 0);
}

void Light::SetPointLight(int index, Vector3 pos, Vector4 color, float range)
{
	if (index < 0) return;
	if (index >= PointMax)return;
	lightData.pointLight[index].index = (float)index;
	lightData.pointLight[index].range = range;
	lightData.pointLight[index].type = 1.0f;
	lightData.pointLight[index].dummy = 0.0f;
	lightData.pointLight[index].pos = Vector4(pos.x, pos.y, pos.z, 0);
	lightData.pointLight[index].color = Vector4(color.x, color.y, color.z, color.w);

}
void Light::SetSpotLight(int index, Vector3 pos, Vector4 color, Vector3 dir,
	float range, float innerCorn, float outerCorn)
{
	if (index < 0) return;
	if (index >= SpotMax)return;
	float d = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
	if (d > 0) {
		dir.x /= d; dir.y /= d; dir.z /= d;
	}

	lightData.spotLight[index].index = (float)index;
	lightData.spotLight[index].range = range;
	lightData.spotLight[index].type = 1.0f;
	lightData.spotLight[index].innerCorn = innerCorn;
	lightData.spotLight[index].outerCorn = outerCorn;
	lightData.spotLight[index].dummy0 = 0.0f;
	lightData.spotLight[index].dummy1 = 0.0f;
	lightData.spotLight[index].dummy2 = 0.0f;

	lightData.spotLight[index].pos = Vector4(pos.x, pos.y, pos.z, 0);
	lightData.spotLight[index].color = Vector4(color.x, color.y, color.z, color.w);
	lightData.spotLight[index].dir = Vector4(dir.x, dir.y, dir.z, 0);
}

void Light::Activate(const int slot, const bool vs, const bool ps)
{
	lightConstant->data = lightData;
	lightConstant->Activate(slot, vs, ps);
}

