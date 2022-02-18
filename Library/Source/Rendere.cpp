#include "../Header/Renderer.h"
#include <d3d11.h>
#include <vector>
#include "../Header/Shader.h"
#include "../Header/AIGUI.h"
#include "../Header/misc.h"






void Renderer::CreateVertexBuffer(const void* _vertices,const u_int _numVertex)
{
	ID3D11Device* device = AIGUI::GetDevice();
	HRESULT hr = S_OK;

	{
		D3D11_BUFFER_DESC vd;
		ZeroMemory(&vd, sizeof(vd));
		vd.Usage = D3D11_USAGE_DEFAULT;
		vd.ByteWidth = sizeof(_vertices) * _numVertex;
		vd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//vd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vd.MiscFlags = 0;
		vd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA sub = {};
		sub.pSysMem = _vertices;
		sub.SysMemPitch = 0;
		sub.SysMemSlicePitch = 0;

		hr = device->CreateBuffer(&vd, &sub, vertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	}
}

/// <summary>
/// IndexBuffer�̍쐬
/// </summary>
/// <param name="_indices"> IndexDatas </param>
/// <param name="_numIndex"> sizeof(indices) </param>
void Renderer::CreateIndexBuffer(const u_int* _indices,const u_int _numIndex)
{
	ID3D11Device* device = AIGUI::GetDevice();
	HRESULT hr = S_OK;

	{
		D3D11_BUFFER_DESC id;
		ZeroMemory(&id, sizeof(id));
		id.ByteWidth = sizeof(u_int) * _numIndex;
		id.Usage = D3D11_USAGE_DEFAULT;
		id.BindFlags = D3D11_BIND_INDEX_BUFFER;
		id.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sub;
		ZeroMemory(&sub, sizeof(sub));
		sub.pSysMem = _indices;

		hr = device->CreateBuffer(&id, &sub, indexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}

}
