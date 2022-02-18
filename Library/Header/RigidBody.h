#pragma once
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include "Component.h"
#include "Transform.h"

// TODO : ����v���C���[�𓮂����Ȃ�����@���ꂩ��g���@DX11BulletGame2�Q�l
class RigidBody : public Component
{
public:
	RigidBody() = default;
	

	void Initialize(const std::shared_ptr<ExistObject> existObject) override;

	void DebugUI() override;

	void Update() override;

	void SetPosition() {
		//TODO �O�̃t���[���Ƃ̍����ɂ���
		const btVector3 difference(transform.lock()->GetDifferencePosition().x, transform.lock()->GetDifferencePosition().y, transform.lock()->GetDifferencePosition().z);

		bulletData.rigidBody->applyCentralImpulse(difference);


	}
	
	btRigidBody* GetRigidBody() const { return bulletData.rigidBody.get(); }
	btDefaultMotionState* GetDefaultMotionState() const { return defaultMotionState.get(); }
	void ReleaseRigidBody() { bulletData.rigidBody.release(); }
	void ReleaseDefaultMotionState() { defaultMotionState.reset(); defaultMotionState = nullptr; }
	void ReleaseCollisionShape() { collisionShape.reset(); collisionShape = nullptr; }
private:
	void CreateRigidBody();
	void CreatePairChachingGhostObject();

private:
	enum ShapeType
	{
		BOX,
		CAPSULE,
		CYLINDER,

	};
	struct BulletData
	{
		btVector3 btPosition; // ���S���W
		btScalar mass = 0.03; // ����
		btScalar restitution = 0.8; // �����W��
		ShapeType shapeType;
		std::unique_ptr<btRigidBody> rigidBody; // ���̃I�u�W�F�N�g
	};
	std::shared_ptr<btCollisionShape> collisionShape;
	std::shared_ptr<btGhostObject> ghostObject;
	std::shared_ptr<btDefaultMotionState> defaultMotionState;
	BulletData bulletData;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive);
};

CEREAL_REGISTER_TYPE(RigidBody)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, RigidBody)

template <class Archive>
void RigidBody::serialize(Archive& archive)
{
	archive(cereal::base_class<Component>(this),bulletData.mass,bulletData.restitution);
}
