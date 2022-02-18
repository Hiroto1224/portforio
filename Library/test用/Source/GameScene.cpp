#include "../Header/GameScene.h"
#include "../Header/BlackBoard.h"
#include "../imgui-docking/imgui.h"
#include "../../myLibScripts/Header/RenderObjectManager.h"
#include "../../myLibScripts/Header/Transform.h"
void GameScene::Initialize()
{
	// Handlerの初期化
	frameBufferHandler = std::make_unique<FrameBufferHandler>();
	samplerStateHandler = std::make_unique<SamplerStateHandler>();
	rasterizerHandler = std::make_unique<RasterizerHandler>();
	depthStencilStateHandler = std::make_unique<DepthStencilStateHandler>();


	const std::shared_ptr<StartingOver::FrameBuffer> frameBuffer = std::make_shared<StartingOver::FrameBuffer>(
		StartingOver::DirectX11Manager::GetScreenWidth(), StartingOver::DirectX11Manager::GetScreenHeight(), false, 8,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
	frameBufferHandler->ReceiveFrameBuffer("default", frameBuffer);
	const std::shared_ptr<StartingOver::FrameBuffer> shadowMap = std::make_shared<StartingOver::FrameBuffer>(
		1024 * 5, 1024 * 5, false, 1, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_R32_TYPELESS);
	frameBufferHandler->ReceiveFrameBuffer("shadowMap", shadowMap);


	std::shared_ptr<StartingOver::SamplerState> samplerState[3];
	samplerState[0] = std::make_shared<StartingOver::SamplerState>(D3D11_FILTER_MIN_MAG_MIP_POINT);
	samplerStateHandler->ReceiveSamplerState("POINT", samplerState[0]);
	samplerState[1] = std::make_shared<StartingOver::SamplerState>(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	samplerStateHandler->ReceiveSamplerState("LINEAR", samplerState[1]);
	samplerState[2] = std::make_shared<StartingOver::SamplerState>(D3D11_FILTER_ANISOTROPIC);
	samplerStateHandler->ReceiveSamplerState("ANISOTROPIC", samplerState[2]);

	const std::shared_ptr<StartingOver::Rasterizer> rasterizer = std::make_shared<StartingOver::Rasterizer>();
	rasterizerHandler->ReceiveRasterizer("default", rasterizer);


	const std::shared_ptr<StartingOver::DepthStencilState> depthStencilState = std::make_shared<StartingOver::DepthStencilState>(true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS);
	depthStencilStateHandler->ReceiveDepthStencilState("default", depthStencilState);

	// managerの初期化
	agentManager = std::make_unique<AgentManager>();
	objectManager = std::make_unique<ObjectManager>();

	rendererConstants = std::make_unique<RendererConstants>();
	rendererConstants->Initialize();

	RenderObjectManager::Initialize();


	bulletPhysics = std::make_unique<BulletPhysics>();
	bulletPhysics->Initialize();

	// lightの設定
	light = std::make_unique<Light>();
	light->Initialize();
	light->SetPointLight(0,Vector3(0.0, 10, 0), Vector4(0.0f, 0.0f, 0.0f, 1.0f), 5);
	
	light->SetDirectionalLight(Vector3(1.0f,1.0f,0.0f), Vector4(1,1,1,1.0f));
	light->SetAmbient(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

	// カメラの設置
	cameraObject = std::make_shared<ExistObject>();
	std::shared_ptr<Transform> transform = std::make_shared<Transform>();
	transform->Initialize();
	transform->SetPosition(Vector3(0.0f, 5.0f, 10.0f));
	cameraObject->SetTransform(transform);

	sceneCamera = std::make_shared<Camera>();
	sceneCamera->Initialize(cameraObject);

	BlackBoard::SetCamera(sceneCamera);

	// スカイマップの設置
	skyMapObject = std::make_shared<ExistObject>();
	skyMapObject->SetTransform(std::make_shared<Transform>());
	skyMapObject->SetName("skyMap");
	skyMapObject->SetTag("skyMap");
	skyMap = std::make_shared<SkyMap>();
	skyMap->Initialize((std::static_pointer_cast<ExistObject>(skyMapObject->shared_from_this())));
	skyMap->transform.lock()->SetScale(Vector3(10000, 10000, 10000));



}

void GameScene::Update()
{
	sceneCamera->Update();

	agentManager->Update();
	objectManager->Update();

	bulletPhysics->Update();


	ImGui::Begin("Scene Data");
	{
		Vector4 dir = light->GetDirectionalLightDirection();
		float directional[3] = { dir.x,dir.y,dir.z };

		ImGui::InputFloat3("x : %f y : %f z : %f", directional);
		Vector3 direction = { directional[0],directional[1] ,directional[2] };
		light->SetDirectionalLightDirection(direction);

	}
	ImGui::End();
}

void GameScene::Render()
{
	StartingOver::SetBlendMode(StartingOver::Blender::BS_NONE);
	depthStencilStateHandler->Activate("default");
	samplerStateHandler->Activate("POINT",0,false);
	samplerStateHandler->Activate("LINEAR", 1, false);
	samplerStateHandler->Activate("ANISOTROPIC", 2, false);

	light->Activate(2, true, true);

	sceneCamera->Activate(1, true, true);

	rendererConstants->Activate(3, true, true);

	frameBufferHandler->Activate("shadowMap");

	// 影

	frameBufferHandler->InActivate("shadowMap");

	//未使用　要検討
	//RenderObjectManager::RenderShadow();

	frameBufferHandler->Activate("default");

	constexpr float angle = 12 / 24 * 360;
	const float bias = 0.2f;
	const float amplitude = 10.0f;
	float alpha = std::min<float>(1, std::max<float>(bias, amplitude * -cosf(angle * 0.01745f)));
	

	float sky_dimension = 1000;
	skyMap->Render();

	// モデル
	//blockManager->Render();
	RenderObjectManager::Render();

	//デバッグ用現在未使用
	//bulletPhysics->Render();

	frameBufferHandler->InActivate("default");

	rasterizerHandler->Blit("default",frameBufferHandler->GetFrameBuffer("default").lock()->GetRenderTargetShaderResourceView(),
		Vector2(0, 0), Vector2(1920, 1080));

	samplerStateHandler->InActivate("POINT");
	samplerStateHandler->InActivate("LINEAR");
	samplerStateHandler->InActivate("ANISOTROPIC");

	depthStencilStateHandler->InActivate("default");



}



std::shared_ptr<ExistObject> GameScene::Find(const std::string& name)
{
	for(auto & [fst, snd] : objectList)
	{
		for(auto& object : snd)
		{
			//if(object->GetName() == name)
			{
				return object;
			}
		}
	}
	std::shared_ptr<ExistObject> null;
	return null;
}

#include "../myLibScripts/Header/Transform.h"

std::shared_ptr<ExistObject> GameScene::CreateObject()
{
	std::shared_ptr<ExistObject> existObject = std::make_shared<ExistObject>();
	existObject->SetTransform(std::make_shared<Transform>());
	existObject->SetName("default Object");
	existObject->SetTag("default");
	decltype(objectList)::iterator it = objectList.find("Default");
	if(it == objectList.end())
	{
		std::vector<std::shared_ptr<ExistObject>> create;
		create.push_back(existObject);
		objectList.insert(std::make_pair("Default", create));
	}
	else
	{
		objectList.at("Default").push_back(existObject);
	}
	return existObject;
}

std::map<std::string/* object type */, std::vector<std::shared_ptr<ExistObject>>> GameScene::ObjectList()
{
	return objectList;
}

void GameScene::AddRigidBody(std::shared_ptr<RigidBody> rigidBody)
{
	bulletPhysics->AddRigidBody(rigidBody);
}



void GameScene::load()
{
	std::ifstream stream;
	stream.open("testsave.dat", std::ios::binary);
	cereal::BinaryInputArchive archive(stream);
	archive(objectList);

	for (auto& objects : SceneManager::GetActiveScene()->ObjectList())
	{
		for (auto& object : objects.second)
		{
			
			object->Initialize();
			
		}
	}
}


void GameScene::save()
{
	std::ofstream stream;
	stream.open("testsave.dat", std::ios::binary);
	cereal::BinaryOutputArchive archive(stream);
	archive(objectList);
}
