#include "../Header/RigidBody.h"

#include "../Header/ExistObject.h"

#include <imgui-docking/imgui.h>
#include "../Header/SceneManager.h"

void RigidBody::Initialize(const std::shared_ptr<ExistObject> object)
{
	existObject = object;
	transform = object->GetTransform();
	
#ifdef _Release
	CreateRigidBody();
#endif


}

void RigidBody::Update()
{
	btVector3 pos = bulletData.rigidBody->getCenterOfMassPosition();
	transform.lock()->SetPosition(pos[0], pos[1], pos[2]);
}

void RigidBody::DebugUI()
{
	ImGui::Text("RigidBody");

	float mass = bulletData.mass;
	ImGui::DragFloat(u8"����", &mass, 0.05f, -FLT_MAX, FLT_MAX);
	bulletData.mass = mass;
	float restitution = bulletData.restitution;
	ImGui::DragFloat(u8"�����W��", &restitution, 0.05f, -FLT_MAX, FLT_MAX);
	bulletData.restitution = restitution;

	if(ImGui::Button("SetRigidBody"))
	{
		CreateRigidBody();
	}
}


void RigidBody::CreateRigidBody()
{
	const Vector3 position = transform.lock()->GetPosition();
	bulletData.btPosition = btVector3(position.x, position.y, position.z);

	// ���`���ݒ�
	
	Vector3 scale = transform.lock()->GetScale();
	Vector3 size = transform.lock()->GetModelSize();

	switch (bulletData.shapeType)
	{
	case BOX:
		collisionShape = std::make_shared<btBoxShape>(btVector3(scale.x / size.x / 2, scale.y / size.y / 2, scale.z / size.z / 2));
		break;
	case CAPSULE:
		collisionShape = std::make_shared<btCapsuleShape>(scale.x / size.x / 2, scale.y / size.y / 2);
		break;
	case CYLINDER:
		collisionShape = std::make_shared<btCylinderShape>(btVector3(scale.x / size.x / 2, scale.y / size.y / 2, scale.z / size.z / 2));

		break;
	}
	ghostObject = std::make_shared<btGhostObject>();

		// ���̏����ʒu�E�p��
	const Vector3 scaleTmp = transform.lock()->GetScale();
	const btQuaternion quaternion(scaleTmp.x, scaleTmp.y, scaleTmp.z, 1);
	defaultMotionState = std::make_shared<btDefaultMotionState>(btTransform(quaternion, bulletData.btPosition));

	// �������[�����g�̌v�Z
	// �ړ�����I�u�W�F�N�g�̎��v�Z
	const bool isMove = (bulletData.mass != 0.0f);
	btVector3 inertia(0, 0, 0);
	if (isMove)
	{
		ghostObject->setCollisionShape(collisionShape.get());
		ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		ghostObject->setWorldTransform(btTransform(quaternion, bulletData.btPosition));

		collisionShape->calculateLocalInertia(bulletData.mass, inertia);
	}
	btRigidBody::btRigidBodyConstructionInfo btRigidBodyConstruction(bulletData.mass, defaultMotionState.get(), collisionShape.get(), inertia);
	// ���̃I�u�W�F�N�g����
	bulletData.rigidBody = std::make_unique<btRigidBody>(btRigidBodyConstruction);
	bulletData.rigidBody->setRestitution(bulletData.restitution);

	SceneManager::GetActiveScene()->AddRigidBody(std::static_pointer_cast<RigidBody>(shared_from_this()));
}
