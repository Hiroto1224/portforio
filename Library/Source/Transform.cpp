#include "../Header/Transform.h"
#include "../Header/Component.h"
#include "../Header/ExistObject.h"

void Transform::Initialize()
{
	SetPosition(Vector3(0.0f, 0.0f, 0.0f));
	SetEulerAngles(Vector3(0.0f, 0.0f, 0.0f));
	SetScale(Vector3(1.0f, 1.0f, 1.0f));

}


void Transform::Initialize(const std::shared_ptr<ExistObject> object)
{
	existObject = object;
	object->SetTransform(std::make_shared<Transform>(*this));
	transform = std::make_shared<Transform>(*this);
	Initialize();
}

void Transform::DebugUI()
{
}



void Transform::SetPosition(const Vector3 setPosition)
{
	data.position = setPosition;
	data.translationMatrix = Matrix::CreateTranslation(data.position);
	data.worldMatrix = data.scaleMatrix * data.rotationMatrix * data.translationMatrix;
	beforeData = data;
}

void Transform::SetPosition(const float* setPosition)
{
	data.position = Vector3(setPosition[0], setPosition[1], setPosition[2]);
	data.translationMatrix = Matrix::CreateTranslation(data.position);
	data.worldMatrix = data.scaleMatrix * data.rotationMatrix * data.translationMatrix;
	beforeData = data;

}

void Transform::SetPosition(const float x, const float y, const float z)
{
	data.position = Vector3(x, y, z);
	data.translationMatrix = Matrix::CreateTranslation(data.position);
	data.worldMatrix = data.scaleMatrix * data.rotationMatrix * data.translationMatrix;
	beforeData = data;

}

void Transform::SetScale(const Vector3 setScale)
{
	data.scale = setScale - data.modelSize;
	data.scaleMatrix = Matrix::CreateScale(data.scale);
	data.worldMatrix = data.scaleMatrix * data.rotationMatrix * data.translationMatrix;
	beforeData = data;

}

void Transform::SetScale(const float* setScale)
{
	data.scale = Vector3(setScale[0], setScale[1], setScale[2]);
	data.scaleMatrix = Matrix::CreateScale(data.scale);
	data.worldMatrix = data.scaleMatrix * data.rotationMatrix * data.translationMatrix;
	beforeData = data;

}

void Transform::SetScale(const float x, const float y, const float z)
{
	data.scale = Vector3(x, y, z);
	data.scaleMatrix = Matrix::CreateScale(data.scale);
	data.worldMatrix = data.scaleMatrix * data.rotationMatrix * data.translationMatrix;

	beforeData = data;

}

void Transform::SetModelSize(const Vector3 setModelSize)
{
	data.modelSize = setModelSize;
	beforeData = data;

}


void Transform::SetEulerAngles(const Vector3 setAngles)
{
	data.eulerAngles = setAngles;
	SetRotation(Quaternion::Euler(data.eulerAngles));
	data.eulerAngles = data.rotation.GetEuler();
	beforeData = data;

}

void Transform::SetEulerAngles(const float* setAngles)
{
	data.eulerAngles = Vector3(setAngles[0], setAngles[1], setAngles[2]);
	SetRotation(Quaternion::Euler(data.eulerAngles));
	data.eulerAngles = data.rotation.GetEuler();
	beforeData = data;

}

void Transform::SetEulerAngles(const float x, const float y, const float z)
{
	data.eulerAngles = Vector3(x, y, z);
	SetRotation(Quaternion::Euler(data.eulerAngles));
	data.eulerAngles = data.rotation.GetEuler();
	beforeData = data;

}

void Transform::SetRotation(const Quaternion quaternion)
{
	data.rotation = quaternion;
	data.rotationMatrix = Matrix::CreateFromQuaternion(data.rotation);

	data.worldMatrix = data.scaleMatrix * data.rotationMatrix * data.translationMatrix;


	data.forward = Vector3::Transform(Vector3::Foward(), data.rotationMatrix).Normalize();

	data.right = Vector3::Transform(Vector3::Right(), data.rotationMatrix).Normalize();

	data.up = Vector3::Transform(Vector3::Up(), data.rotationMatrix).Normalize();

	beforeData = data;

}








Vector3 Transform::GetPosition() const noexcept
{
	return data.position;
}

Vector3 Transform::GetScale() const noexcept
{
	return data.scale;
}

Vector3 Transform::GetModelSize() const noexcept
{
	return data.modelSize;
}


Vector3 Transform::GetEulerAngles() const noexcept
{
	return  data.eulerAngles;
}

Vector3 Transform::GetRight() const noexcept
{
	return data.right;
}

Vector3 Transform::GetUp() const noexcept
{
	return data.up;
}

Vector3 Transform::GetForward() const noexcept
{
	return data.forward;
}

Quaternion Transform::GetRotation() const noexcept 
{
	return data.rotation;
}

Matrix Transform::GetWorldMatrix() const noexcept
{
	return data.worldMatrix;
}



Vector3 Transform::GetBeforePosition() const noexcept
{
	return beforeData.position;
}

Vector3 Transform::GetBeforeScale() const noexcept
{
	return beforeData.scale;
}

Vector3 Transform::GetBeforeModelSize() const noexcept
{
	return beforeData.modelSize;
}


Vector3 Transform::GetBeforeEulerAngles() const noexcept
{
	return  beforeData.eulerAngles;
}

Vector3 Transform::GetBeforeRight() const noexcept
{
	return beforeData.right;
}

Vector3 Transform::GetBeforeUp() const noexcept
{
	return beforeData.up;
}
 
Vector3 Transform::GetBeforeForward() const noexcept
{
	return beforeData.forward;
}

Quaternion Transform::GetBeforeRotation() const noexcept
{ 
	return beforeData.rotation;
}

Matrix Transform::GetBeforeWorldMatrix() const noexcept
{
	return beforeData.worldMatrix;
}


Vector3 Transform::GetDifferencePosition() const noexcept
{
	return data.position - beforeData.position;
}
