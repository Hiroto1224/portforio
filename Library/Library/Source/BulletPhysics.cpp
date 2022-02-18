#include "../Header/BulletPhysics.h"
#include "../Header/StartingOver.h"
#include <imgui-docking/imgui.h>
#include "../HideSeek/Header/BlackBoard.h"
void BulletPhysics::Initialize()
{
	
	// �Փˌ��o���@�̑I��(�f�t�H���g)
	config = std::make_unique<btDefaultCollisionConfiguration>();

	dispatcher = std::make_unique<btCollisionDispatcher>(config.get());

	// �u���[�h�t�F�[�Y�@�̐ݒ�(Dynamic AABB tree method)
	broadphase = std::make_unique<btDbvtBroadphase>();

	// �S��(���̊ԃ����N)�̃\���o�ݒ�
	solver = std::make_unique<btSequentialImpulseConstraintSolver>();

	// Bullet�̃��[���h�쐬
	dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), config.get());
	dynamicsWorld->setGravity(btVector3(0.0f, -9.8, 0));

	//�S�[�X�g�y�A�R�[���o�b�N
	ghostPairCallback = std::make_unique<btGhostPairCallback>();
	dynamicsWorld->getPairCache()->setInternalGhostPairCallback(ghostPairCallback.get());

	dynamicsWorld->getDispatchInfo().m_allowedCcdPenetration *= 0.1f;
	
	yourOwnFilterCallback = std::make_unique<YourOwnFilterCallback>();
	dynamicsWorld->getPairCache()->setOverlapFilterCallback(yourOwnFilterCallback.get());

	//iBulletDebugDrawDx11 = std::make_unique<IBulletDebugDrawDX11>();
	//iBulletDebugDrawDx11->Setup(StartingOver::DirectX11Manager::GetDevice(), StartingOver::DirectX11Manager::GetDeviceContext());
	//iBulletDebugDrawDx11->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawContactPoints);
	//dynamicsWorld->setDebugDrawer(iBulletDebugDrawDx11.get());


#ifdef _Release
	activePhysics = true;

#endif

}

void BulletPhysics::Update() 
{
	const float deltaTime = StartingOver::DirectX11Manager::hrTimer.timeInterval();
	DebugUI();
	if (activePhysics)
	{
		if (dynamicsWorld)
		{
			for (const auto& rigidBody : stackRigidBody)
			{
				rigidBody->SetPosition();
			}
			dynamicsWorld->stepSimulation(deltaTime);
			for (const auto& rigidBody : stackRigidBody)
			{
				rigidBody->Update();
			}
		}
	}

	

}

void BulletPhysics::Render()
{
	ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

	//iBulletDebugDrawDx11-

	iBulletDebugDrawDx11->SetProjMatrix(BlackBoard::GetReadonlyCamera()->GetProjection());
	iBulletDebugDrawDx11->SetViewMatrix(BlackBoard::GetReadonlyCamera()->GetView());
	dynamicsWorld->debugDrawWorld();
}


void BulletPhysics::DebugUI()
{
	ImGui::Begin("BulletPhysics");
	{
		if(ImGui::Button("ChangeActivePhysics"))
		{
			activePhysics = !activePhysics;
		}
	}

	ImGui::End();
}


void BulletPhysics::AddRigidBody(std::shared_ptr<RigidBody> rigidBody)
{
	// ���[���h�ɍ��̃I�u�W�F�N�g��ǉ�
	dynamicsWorld->addRigidBody(rigidBody->GetRigidBody());

	stackRigidBody.push_back(rigidBody);

}


void BulletPhysics::Release()
{
	if (stackRigidBody.size() != 0)
	{
		/*for (auto& rigidBody : stackRigidBody)
		{
		
			rigidBody->ReleaseDefaultMotionState();
			rigidBody->ReleaseCollisionShape();

			dynamicsWorld->removeRigidBody(rigidBody->GetRigidBody());
			rigidBody->ReleaseRigidBody();
		}*/

		int i;
		for (i = dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
		{
			dynamicsWorld->removeConstraint(dynamicsWorld->getConstraint(i));
		}
		for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}
		for (auto& rigidBody : stackRigidBody)
		{
			rigidBody->ReleaseDefaultMotionState();
			rigidBody->ReleaseCollisionShape();
			rigidBody->ReleaseRigidBody();
		}

		stackRigidBody.clear();

		delete dynamicsWorld->getBroadphase();
		broadphase.release();
	
	}
}
