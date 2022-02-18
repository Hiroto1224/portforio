#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <memory>

#include "RigidBody.h"
#include "BulletDebugDraw.h"

struct YourOwnFilterCallback;

class BulletPhysics
{
public:
	BulletPhysics() = default;
	virtual ~BulletPhysics() { Release(); };

	void Initialize();
	void Update();
	void Render();
	
	void AddRigidBody(std::shared_ptr<RigidBody> rigidBody);

	void DebugUI();

	void Release(); // �o���b�g�͉�������Ԃɂ��Ȃ��Ƃ��߂�����
private:
	// �����v�Z���鐢�E(�����ɕ��̂����Ă�)
	std::unique_ptr<btDynamicsWorld> dynamicsWorld;
	// Bullet Debug draw
	std::unique_ptr<IBulletDebugDrawDX11> iBulletDebugDrawDx11;
	//btGhostPairCallback�́AbtBroadphaseInterface����
	//btGhostObject�ւ̃I�[�o�[���b�v�y�A�̒ǉ��ƍ폜���C���^�[�t�F�C�X���ē]�����܂��B
	std::unique_ptr<btGhostPairCallback> ghostPairCallback;
	// �Փ˂̃t�B���^�����O
	std::unique_ptr<YourOwnFilterCallback> yourOwnFilterCallback;

	// �Փˌ��o���@�̐ݒ�(��{�I�Ƀf�t�H���g��ݒ�)
	std::unique_ptr<btDefaultCollisionConfiguration> config;
	//
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btBroadphaseInterface> broadphase;
	std::unique_ptr<btConstraintSolver> solver;

	std::vector<std::shared_ptr<RigidBody>> stackRigidBody;

	bool activePhysics = false;


};

//�u���[�h�t�F�[�Y�t�B���^�[�R�[���o�b�N
struct YourOwnFilterCallback : public btOverlapFilterCallback
{
	// return true when pairs need collision
	// btOverlapFilterCallback�ŏ������n�܂�̊֐������珬����
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
	{
		bool collides = (proxy0->m_collisionFilterGroup &proxy1->m_collisionFilterMask) != 0;
		collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);

		//add some additional logic here that modified 'collides'
		return collides;
	}
};