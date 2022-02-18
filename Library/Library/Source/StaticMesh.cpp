#include "../Header/StaticMesh.h"
#include "../Header/FileFunction.h"

#include "imgui-docking/imgui.h"
#include "../Header/RenderObjectManager.h"
#include "../Header/Transform.h"

void StaticMesh::Initialize(const std::shared_ptr<ExistObject> object)
{
	RenderObjectManager::AddRenderObject(std::static_pointer_cast<StaticMesh>(shared_from_this()));

	existObject = object;
	transform = object->GetTransform();

	if (!vertexShader)
	{
		vertexShader = std::make_unique<StartingOver::VertexShader<Vertex>>("Shader/StaticMesh_vs.cso");
	}
	if (!pixelShader)
	{
		pixelShader = std::make_unique<StartingOver::PixelShader>("Shader/StaticMesh_ps.cso");
	}
	if (!shadowVertexShader)
	{
		shadowVertexShader = std::make_unique<StartingOver::VertexShader<Vertex>>("Shader/StaticMeshShadowCast_vs.cso");
	}

	if(!fileName.empty())
	{
		Load();
	}
}

void StaticMesh::Load()
{
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

	Vector3 size = substance->meshes.at(0).boundingBox.max - substance->meshes.at(0).boundingBox.min;
	transform.lock()->SetModelSize(size);
}

void StaticMesh::DebugUI()
{
	std::string filePath;
	if (ImGui::Button(u8"ƒ‚ƒfƒ‹“Ç‚Ýž‚Ý"))
	{
		fileName = FileFunction::OpenExplorer();

		if (!fileName.empty())
		{
			Load();
		}
	}
	ImGui::Text(fileName.c_str());

}


void StaticMesh::Render()
{
	ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

	//RenderShadow();

	vertexShader->Activate();
	pixelShader->Activate();

	for (const auto mesh : substance->meshes)
	{
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;

		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const Matrix world = transform.lock()->GetWorldMatrix();
		DirectX::XMStoreFloat4x4
		(
			&constantBuffer->data.world,
			DirectX::XMMatrixTranspose(
				DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
				DirectX::XMLoadFloat4x4(&substance->systemUnitTransform) *
				DirectX::XMLoadFloat4x4(&substance->axisSystemTransform) *
				DirectX::XMLoadFloat4x4(&world))
		);


		for(const StartingOver::Subset& subset : mesh.subsets)
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

void StaticMesh::RenderShadow()
{
	shadowVertexShader->Activate();

	ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

	for (const StartingOver::Mesh<Vertex>& mesh : substance->meshes)
	{
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;

		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const Matrix world = transform.lock()->GetWorldMatrix();

		DirectX::XMStoreFloat4x4
		(
			&constantBuffer->data.world,
			DirectX::XMMatrixTranspose(
				DirectX::XMLoadFloat4x4(&mesh.globalTransform) *
				DirectX::XMLoadFloat4x4(&substance->systemUnitTransform) *
				DirectX::XMLoadFloat4x4(&substance->axisSystemTransform) *
				DirectX::XMLoadFloat4x4(&world))
		);


		for (const StartingOver::Subset& subset : mesh.subsets)
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
