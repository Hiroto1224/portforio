#include "../Header/SkyMap.h"
#include "../Header/Shader.h"
#include "../Header/Transform.h"

void SkyMap::Initialize(const std::shared_ptr<ExistObject> object)
{
	existObject = object;
	transform = object->GetTransform();
	transform.lock()->Initialize();

	substance = std::make_unique<StartingOver::Geometric::Substance<Vertex>>("model/Skymaps/sphere.fbx", false,-1);
	substance->CreateRenderObjects(true);
	fileName = "model/Skymaps/envmap_miramar/envmap_miramar.dds";
	StartingOver::LoadTextureFromFile(fileName, shaderResourceView.GetAddressOf(), true, true);

	constantBuffer = std::make_unique<StartingOver::ConstantBuffer<ShaderConstants>>();


	if (!vertexShader)
	{
		vertexShader = std::make_unique<StartingOver::VertexShader<Vertex>>("Shader/SkyMap_vs.cso");
	}
	if (!pixelShader)
	{
		pixelShader = std::make_unique<StartingOver::PixelShader>("Shader/SkyMap_ps.cso");
	}
	Vector3 size = substance->meshes.at(0).boundingBox.max - substance->meshes.at(0).boundingBox.min;
	transform.lock()->SetModelSize(size);
}


void SkyMap::Render()
{
	ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();

	vertexShader->Activate();
	pixelShader->Activate();

	context->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
	
	for(StartingOver::Geometric::Substance<Vertex>::Mesh &mesh : substance->meshes)
	{
		unsigned int stride = sizeof(Vertex);
		unsigned int offset = 0;

		const Matrix world = transform.lock()->GetWorldMatrix();
		constantBuffer->data.world = world;

		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(),&stride,&offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT,0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (StartingOver::Geometric::Material& material : mesh.materials)
		{
			constantBuffer->data.color = Vector4(1, 1, 1, 1);
			constantBuffer->Activate(0, true, true);

			context->DrawIndexed(material.indexCount, material.indexStart, 0);
		}

	}

	vertexShader->InActivate();
	pixelShader->InActivate();

}
