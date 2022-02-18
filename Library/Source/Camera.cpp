#include "../Header/Input.h"
#include "../Header/Camera.h"
#include "../Header/ExistObject.h"
#include "../Header/StartingOver.h"
#include "../Header/Transform.h"



void Camera::Initialize(const std::shared_ptr<ExistObject> object)
{
	existObject = object;
	transform = object->GetTransform();

	constantBuffer = std::make_unique<StartingOver::ConstantBuffer<Constant>>();

}

void Camera::Update()
{
	const float deltaTime = StartingOver::DirectX11Manager::hrTimer.timeInterval();
	static Vector2 mousePos = { 0.0f,0.0f };
	static Vector2 mouseOldPos = { 0.0f,0.0f };

	if(Input::GetMouseButton(1))
	{
		mousePos = Input::GetMousePos();

		if (mouseOldPos == Vector2(-1, -1)) mouseOldPos = mousePos;

		if(mousePos.x != mouseOldPos.x || mousePos.y != mouseOldPos.y)
		{
			const float disX = (mousePos.x - mouseOldPos.x) * 0.1f;
			const float disY = (mousePos.y - mouseOldPos.y) * 0.1f;

			Vector3 rot = transform.lock()->GetEulerAngles();
			rot.x += disY;
			rot.y += disX;
			rot.z = 0;
			transform.lock()->SetEulerAngles(rot);

			mouseOldPos = mousePos;
		}

		static  float wheelSpeed = 1;
		float mouseWheel = Input::GetMouseWheel();
		if(mouseWheel != 0.0f)
		{
			wheelSpeed += mouseWheel * 3;
			if(wheelSpeed < 5)
			{
				wheelSpeed = 5;
			}
			if (wheelSpeed > 20000)
			{
				wheelSpeed = 20000;
			}
		}
		Vector3 move{};
		if(Input::GetKey(KeyCode::W))
		{
			move += transform.lock()->GetForward() * deltaTime * wheelSpeed;
		}
		if(Input::GetKey(KeyCode::S))
		{
			move -= transform.lock()->GetForward() * deltaTime * wheelSpeed;
		}
		if (Input::GetKey(KeyCode::A))
		{
			move -= transform.lock()->GetRight() * deltaTime * wheelSpeed;
		}
		if (Input::GetKey(KeyCode::D))
		{
			move += transform.lock()->GetRight() * deltaTime * wheelSpeed;
		}
		transform.lock()->SetPosition(transform.lock()->GetPosition() + move);
	}
	else
	{
		mouseOldPos = Vector2(-1, -1);
	}



}


void Camera::Activate(int slot, bool isVS, bool isPS)
{
	fov = DirectX::XMConvertToRadians(30);
	aspectRatio = StartingOver::DirectX11Manager::GetScreenWidth() / StartingOver::DirectX11Manager::GetScreenHeight();
	nearZ = 0.1f;
	farZ = 100000.0f;

	Matrix P;
	if (perspectiveProjection)
	{
		P = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
	}

	Vector3 pos = transform.lock()->GetPosition();
	Vector4 eye = Vector4(pos.x, pos.y, pos.z, 0);
	DirectX::XMVECTOR E = DirectX::XMLoadFloat4(&eye);
	Vector3 forward = transform.lock()->GetForward();
	DirectX::XMVECTOR F = DirectX::XMVectorAdd(E, DirectX::XMLoadFloat3(&forward));
	DirectX::XMVECTOR U = transform.lock()->GetUp();
	Matrix V = DirectX::XMMatrixLookAtLH(E, F, U);

	DirectX::XMVECTOR D = DirectX::XMVector3Normalize(F);
	DirectX::XMStoreFloat4(&constantBuffer->data.position, E);
	DirectX::XMStoreFloat4(&constantBuffer->data.direction, D);

	DirectX::XMStoreFloat4x4(&constantBuffer->data.view, V);
	DirectX::XMStoreFloat4x4(&constantBuffer->data.inverseView, DirectX::XMMatrixInverse(0, V));
	DirectX::XMStoreFloat4x4(&constantBuffer->data.projection, P);
	DirectX::XMStoreFloat4x4(&constantBuffer->data.inverseProjection, DirectX::XMMatrixInverse(0, P));
	DirectX::XMStoreFloat4x4(&constantBuffer->data.viewProjection, V * P);
	XMStoreFloat4x4(&constantBuffer->data.inverseViewProjection, DirectX::XMMatrixInverse(0, V * P));
	constantBuffer->Activate(slot, isVS, isPS);

}



Vector4 Camera::GetDirection() const { return constantBuffer->data.direction; }
Matrix Camera::GetView() const { return constantBuffer->data.view; }
Matrix Camera::GetProjection() const { return constantBuffer->data.projection; }
Matrix Camera::GetViewProjection() const { return constantBuffer->data.viewProjection; }
Matrix Camera::GetInverseView() const { return constantBuffer->data.inverseView; }
Matrix Camera::GetInverseProjection() const { return constantBuffer->data.inverseProjection; }
Matrix Camera::GetInverseViewProjection() const { return constantBuffer->data.inverseViewProjection; }