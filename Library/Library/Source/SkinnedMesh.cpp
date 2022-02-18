#include <Shlwapi.h>

#include "../Header/SkinnedMesh.h"
#include "../Header/StartingOver.h"
#include "../Header/RenderObjectManager.h"
#include "imgui-docking/imgui.h"
#include "../Header/FileFunction.h"
#include "../Header/Transform.h"


void SkinnedMesh::Initialize(const std::shared_ptr<ExistObject> object)
{
	RenderObjectManager::AddRenderObject(std::static_pointer_cast<SkinnedMesh>(shared_from_this()));

	existObject = object;
	transform = object->GetTransform();

	if(!vertexShader)
	{
		vertexShader = std::make_unique<StartingOver::VertexShader<Vertex>>("Shader/SkinnedMesh_vs.cso");
	}

	if(!pixelShader)
	{
		pixelShader = std::make_unique<StartingOver::PixelShader>("Shader/SkinnedMesh_ps.cso");
	}

	if(!shadowVertexShader)
	{
		shadowVertexShader = std::make_unique<StartingOver::VertexShader<Vertex>>("Shader/SkinnedMeshShadowCast_vs.cso");
	}

	animationData = std::make_unique<AnimationData>();

	if(IsActive())
	{
		Load();
	}
}

void SkinnedMesh::Load()
{
	// 読み込んだ事のないモデル読んだ時アニメーションを読み込んでるか確認
	substance = std::make_unique<StartingOver::Substance<Vertex>>(fileName, true);

#if _DEBUG
	substance->CreateRenderObjects(false);
#else
	substance->CreateRenderObjects(true);
#endif

	char mediaDirectory[256];
	strcpy_s(mediaDirectory, fileName.c_str());
	PathFindFileNameA(mediaDirectory)[0] = '\0';
	substance->CreateShaderResourceViews(mediaDirectory);


	constantBuffer = std::make_unique<StartingOver::ConstantBuffer<ShaderConstants>>();

	const Vector3 size = substance->meshes.at(0).boundingBox.max - substance->meshes.at(0).boundingBox.min;
	transform.lock()->SetModelSize(size);
}

void SkinnedMesh::DebugUI()
{
	std::string filePath;
	ImGui::Text("Skinned Mesh");
	if (ImGui::Button(u8"モデル読み込み"))
	{	// .binファイルではなくfbxファイルをロード

		fileName = FileFunction::OpenExplorer();

		if (!fileName.empty())
		{
			Load();
		}
	}
	if (ImGui::Button(u8"アニメーションの追加読み込み"))
	{
		fileName = FileFunction::OpenExplorer();

		if (!fileName.empty())
		{
			substance->AddAnimations(fileName, animationData->animationTimer);
			substance->Save();
		}
	}

	if (IsActive())
	{
		std::vector<std::string> animationNames;
		for (const auto& animation : substance->FindAnimations())
		{
			animationNames.push_back(animation.name.c_str());
		}

		if (!animationNames.empty())
		{
			if (ImGui::BeginCombo("Animation", animationNames.at(animationData->animationClip).data()))
			{
				for (unsigned int n = 0; n < animationNames.size(); n++)
				{
					const bool is_selected = (animationData->animationClip == n);
					if (ImGui::Selectable(animationNames.at(n).data(), is_selected))
						animationData->animationClip = n;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::InputFloat("Animation Speed", &animationData->animationTimer);

			ImGui::SliderInt("Animation Time", &animationData->animationFrame, 0, substance->FindAnimation(animationNames[animationData->animationClip])->skeletalTransforms.size());
		}
		ImGui::Text(fileName.c_str());
	}
	
}

void SkinnedMesh::Animate() const
{
	const float elapsedTime = StartingOver::DirectX11Manager::hrTimer.timeInterval();

	if (substance->animationTakes.size() < 1)
	{
		return;
	}
	static int lastAnimationClip;
	lastAnimationClip = animationData->animationClip;



	animationData->animationFrame = static_cast<size_t>(animationData->animationTick * substance->animationTakes.at(animationData->animationClip).samplingRate);

	if (const size_t lastOfFrame = substance->animationTakes.at(animationData->animationClip).keyframeSize - 1; animationData->animationFrame > lastOfFrame) //end of frames
	{
		animationData->animationFrame = 0;
		animationData->animationTick = 0;
		
	}
	animationData->animationTick += elapsedTime * animationData->animationTimer;

}


void SkinnedMesh::Render()
{

	ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

	vertexShader->Activate();
	pixelShader->Activate();

	Animate();

	const unsigned animationsNum = substance->animationTakes.size();

	for (StartingOver::Mesh<Vertex>& mesh : substance->meshes)
	{
		u_int stride = sizeof(Vertex);
		u_int offset = 0;

		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const Matrix world = transform.lock()->GetWorldMatrix();

		DirectX::XMStoreFloat4x4(
			&constantBuffer->data.world,
			DirectX::XMMatrixTranspose
			(
				DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
				DirectX::XMLoadFloat4x4(&substance->systemUnitTransform) *
				DirectX::XMLoadFloat4x4(&substance->axisSystemTransform) *
				DirectX::XMLoadFloat4x4(&world)
			)
		);


		if (animationsNum > 0)
		{
			const StartingOver::Animation& animation = mesh.animations.at(animationData->animationClip);
			const StartingOver::Skeletal& skeletalTransform = animation.skeletalTransforms.at(animationData->animationFrame);
			Matrix actorTransform;
			if (animation.actorTransforms.size() > 0)
			{
				actorTransform = animation.actorTransforms.at(animationData->animationFrame).transform;
			}
			else
			{
				actorTransform = mesh.globalTransform;
			}

			size_t bonesNum = mesh.offsetTransforms.size();
			_ASSERT_EXPR(bonesNum < MAXBONES, L"'number_of_bones' exceeds MAX_BONES.");
			for (size_t i = 0; i < bonesNum; i++)
			{

				DirectX::XMStoreFloat4x4(&constantBuffer->data.boneTranslation[i],
					DirectX::XMMatrixTranspose
					(
						DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i).transform) *
						DirectX::XMLoadFloat4x4(&skeletalTransform.bones.at(i).transform) *
						DirectX::XMMatrixInverse(0, DirectX::XMLoadFloat4x4(&actorTransform))

					)
				);
			}
		}
		else
		{
			for (int i = 0; i < MAXBONES; i++)
			{
				constantBuffer->data.boneTranslation[i] = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

			}
		}
		for (StartingOver::Subset& subset : mesh.subsets)
		{
			const StartingOver::Material* material = substance->FindMaterial(&subset.name);
			constantBuffer->data.color = material->properties.at(0).color;
			constantBuffer->Activate(0, true, true);

			context->PSSetShaderResources(0, 1, material->properties.at(0).SRView.GetAddressOf());
			context->PSSetShaderResources(1, 1, material->properties.at(1).SRView.GetAddressOf());
			context->PSSetShaderResources(2, 1, material->properties.at(2).SRView.GetAddressOf());
			context->PSSetShaderResources(3, 1, material->properties.at(3).SRView.GetAddressOf());
			context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
		}

	}

	pixelShader->InActivate();

	vertexShader->InActivate();


}


void SkinnedMesh::RenderShadow()
{
	// 使ってない
	ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

	shadowVertexShader->Activate();

	const unsigned animationsNum = substance->animationTakes.size();

	for (StartingOver::Mesh<Vertex>& mesh : substance->meshes)
	{
		u_int stride = sizeof(Vertex);
		u_int offset = 0;

		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DirectX::XMStoreFloat4x4(
			&constantBuffer->data.world,
			DirectX::XMMatrixTranspose
			(
				DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
				DirectX::XMLoadFloat4x4(&substance->systemUnitTransform) *
				DirectX::XMLoadFloat4x4(&substance->axisSystemTransform) *
				DirectX::XMLoadFloat4x4(&constantBuffer->data.world)
			)
		);


		if (animationsNum > 0)
		{
			const StartingOver::Animation& animation = mesh.animations.at(animationData->animationClip);
			const StartingOver::Skeletal& skeletalTransform = animation.skeletalTransforms.at(animationData->animationFrame);
			Matrix actorTransform;
			if (animation.actorTransforms.size() > 0)
			{
				actorTransform = animation.actorTransforms.at(animationData->animationFrame).transform;
			}
			else
			{
				actorTransform = mesh.globalTransform;
			}

			size_t bonesNum = mesh.offsetTransforms.size();
			_ASSERT_EXPR(bonesNum < MAXBONES, L"'number_of_bones' exceeds MAX_BONES.");
			for (size_t i = 0; i < bonesNum; i++)
			{

				DirectX::XMStoreFloat4x4(&constantBuffer->data.boneTranslation[i],
					DirectX::XMMatrixTranspose
					(
						DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i).transform) *
						DirectX::XMLoadFloat4x4(&skeletalTransform.bones.at(i).transform) *
						DirectX::XMMatrixInverse(0, DirectX::XMLoadFloat4x4(&actorTransform))

					)
				);
			}
		}
		else
		{
			for (int i = 0; i < MAXBONES; i++)
			{
				constantBuffer->data.boneTranslation[i] = DirectX::XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

			}
		}
		for (StartingOver::Subset& subset : mesh.subsets)
		{
			const StartingOver::Material* material = substance->FindMaterial(&subset.name);
			constantBuffer->data.color = material->properties.at(0).color;
			constantBuffer->Activate(0, true, true);

			context->PSSetShaderResources(0, 1, material->properties.at(0).SRView.GetAddressOf());
			context->PSSetShaderResources(1, 1, material->properties.at(1).SRView.GetAddressOf());
			context->PSSetShaderResources(2, 1, material->properties.at(2).SRView.GetAddressOf());
			context->PSSetShaderResources(3, 1, material->properties.at(3).SRView.GetAddressOf());
			context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
		}

	}

	shadowVertexShader->InActivate();

}
