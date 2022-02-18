#pragma once


#include <d3d11.h>
#include <wrl.h>
#include "misc.h"
#include "DirectXSystem.h"

namespace StartingOver
{

	template <class T>
	class ConstantBuffer
	{
	public:
		T data;
		ConstantBuffer()
		{
			_ASSERT_EXPR(sizeof(T) % 16 == 0, L"constant buffer's need to be 16 byte aligned");

			ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();
			HRESULT hr = S_OK;
			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.ByteWidth = sizeof(T);
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;

			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			hr = device->CreateBuffer(&bufferDesc, 0, bufferObject.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		}

		virtual ~ConstantBuffer() = default;
		ConstantBuffer(ConstantBuffer&) = delete;
		ConstantBuffer& operator = (ConstantBuffer&) = delete;

		void Activate(int slot,bool vs,bool ps)
		{
			HRESULT hr = S_OK;
			ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
			D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
			D3D11_MAPPED_SUBRESOURCE mappedBuffer;

			hr = context->Map(bufferObject.Get(), 0, map, 0, &mappedBuffer);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			memcpy_s(mappedBuffer.pData, sizeof(T), &data, sizeof(T));
			context->Unmap(bufferObject.Get(), 0);

			if(vs)
			{
				context->VSSetConstantBuffers(slot, 1, bufferObject.GetAddressOf());
			}
			if(ps)
			{
				context->PSSetConstantBuffers(slot, 1, bufferObject.GetAddressOf());

			}
		}


	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> bufferObject;
	};

}
