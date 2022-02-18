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

	void Release(); // バレットは解放を順番にしないとだめだから
private:
	// 物理計算する世界(ここに物体を入れてく)
	std::unique_ptr<btDynamicsWorld> dynamicsWorld;
	// Bullet Debug draw
	std::unique_ptr<IBulletDebugDrawDX11> iBulletDebugDrawDx11;
	//btGhostPairCallbackは、btBroadphaseInterfaceから
	//btGhostObjectへのオーバーラップペアの追加と削除をインターフェイスして転送します。
	std::unique_ptr<btGhostPairCallback> ghostPairCallback;
	// 衝突のフィルタリング
	std::unique_ptr<YourOwnFilterCallback> yourOwnFilterCallback;

	// 衝突検出方法の設定(基本的にデフォルトを設定)
	std::unique_ptr<btDefaultCollisionConfiguration> config;
	//
	std::unique_ptr<btCollisionDispatcher> dispatcher;
	std::unique_ptr<btBroadphaseInterface> broadphase;
	std::unique_ptr<btConstraintSolver> solver;

	std::vector<std::shared_ptr<RigidBody>> stackRigidBody;

	bool activePhysics = false;


};

//ブロードフェーズフィルターコールバック
struct YourOwnFilterCallback : public btOverlapFilterCallback
{
	// return true when pairs need collision
	// btOverlapFilterCallbackで小文字始まりの関数だから小文字
	virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const
	{
		bool collides = (proxy0->m_collisionFilterGroup &proxy1->m_collisionFilterMask) != 0;
		collides = collides && (proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask);

		//add some additional logic here that modified 'collides'
		return collides;
	}
};