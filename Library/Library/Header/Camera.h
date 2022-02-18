#pragma once
#include <memory>

#include "ConstantBuffer.h"
#include "Component.h"

class ExistObject;

class Camera : public Component
{
public:
	Camera() = default;
	~Camera() override = default;

	void Initialize(const std::shared_ptr<ExistObject> object) override;
	
	void Update() override;

	void Activate(int slot, bool vs, bool ps);

	Vector4 GetDirection() const;
	Matrix GetView() const;
	Matrix GetProjection() const;
	Matrix GetViewProjection() const;
	Matrix GetInverseView() const;
	Matrix GetInverseProjection() const;
	Matrix GetInverseViewProjection() const;


private:
	struct Constant
	{
		Vector4 position;
		Vector4 direction;
		Matrix view;
		Matrix projection;
		Matrix viewProjection;
		Matrix inverseView;
		Matrix inverseProjection;
		Matrix inverseViewProjection;
	};


	std::unique_ptr<StartingOver::ConstantBuffer<Constant>> constantBuffer = nullptr;


	float fov = 0.0f;
	bool perspectiveProjection = true;
	float aspectRatio = 1.8f;
	float nearZ = 0.1f;
	float farZ = 1000.0f;



};