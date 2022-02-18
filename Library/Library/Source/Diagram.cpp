#include "../Header/Diagram.h"
#include "../Header/misc.h"



Diagram::Diagram()
{
	ID3D11Device* device = AIGUI::GetDevice();

	constexpr Vertex vertices[] =
	{
		{{-0.5f,0.5f,0.0f},{0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f}},
		{{0.5f,0.5f,0.0f},{0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f}},
		{{-0.5f,-0.5f,0.0f},{0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f}},
		{{0.5f,-0.5f,0.0f},{0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f}}
	};
	const std::vector<u_int> indices = { 0,1,2,3,2,1 };
	CreateVertexBuffer(vertices, sizeof(vertices));
	CreateIndexBuffer(indices.data(), sizeof(indices));
	static constexpr D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	texture = std::make_unique<Texture>();
	texture->MakeDummyTexture(device);
	/*shader = std::make_unique<Shader>();
	shader->Create("Diagram_vs.cso", "Diagram_ps.cso",const_cast<D3D11_INPUT_ELEMENT_DESC*>(inputElementDesc), _countof(inputElementDesc));*/

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}



void Diagram::Initialize(const std::shared_ptr<Data> _data)
{
	data = _data;
}

void Diagram::Render()
{
	ID3D11DeviceContext* context = AIGUI::GetDeviceContext();

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	context->RSGetViewports(&numViewports, &viewport);
	const float screenWidth = viewport.Width;
	const float screenHeight = viewport.Height;

	//shader->Activate();

	Vertex vertexData[4];

	float dx = data->GetPosition().x;
	float dy = data->GetPosition().y;
	float dw = data->GetSize().x;
	float dh = data->GetSize().y;

	// left-top
	float x0 = dx;
	float y0 = dy;
	// right-top
	float x1 = dx + dw;
	float y1 = dy;
	// left-bottom
	float x2 = dx;
	float y2 = dy + dh;
	// right-bottom
	float x3 = dx + dw;
	float y3 = dy + dh;

	// Translate sprite's centre to origin (rotate centre)
	float mx = dx + dw * 0.5f;
	float my = dy + dh * 0.5f;
	x0 -= mx;
	y0 -= my;
	x1 -= mx;
	y1 -= my;
	x2 -= mx;
	y2 -= my;
	x3 -= mx;
	y3 -= my;

	// Rotate each sprite's vertices by angle
	float rx, ry;
	float cos = cosf(data->GetAngle() * 0.01745f);
	float sin = sinf(data->GetAngle() * 0.01745f);
	rx = x0;
	ry = y0;
	x0 = cos * rx + -sin * ry;
	y0 = sin * rx + cos * ry;
	rx = x1;
	ry = y1;
	x1 = cos * rx + -sin * ry;
	y1 = sin * rx + cos * ry;
	rx = x2;
	ry = y2;
	x2 = cos * rx + -sin * ry;
	y2 = sin * rx + cos * ry;
	rx = x3;
	ry = y3;
	x3 = cos * rx + -sin * ry;
	y3 = sin * rx + cos * ry;

	// Translate sprite's centre to original position
	x0 += mx;
	y0 += my;
	x1 += mx;
	y1 += my;
	x2 += mx;
	y2 += my;
	x3 += mx;
	y3 += my;

	// Convert to NDC space
	x0 = 2.0f * x0 / screenWidth - 1.0f;
	y0 = 1.0f - 2.0f * y0 / screenHeight;
	x1 = 2.0f * x1 / screenWidth - 1.0f;
	y1 = 1.0f - 2.0f * y1 / screenHeight;
	x2 = 2.0f * x2 / screenWidth - 1.0f;
	y2 = 1.0f - 2.0f * y2 / screenHeight;
	x3 = 2.0f * x3 / screenWidth - 1.0f;
	y3 = 1.0f - 2.0f * y3 / screenHeight;

	vertexData[0].position.x = x0;
	vertexData[0].position.y = y0;
	vertexData[0].position.z = 0.0f;

	vertexData[1].position.x = x1;
	vertexData[1].position.y = y1;
	vertexData[1].position.z = 0.0f;

	vertexData[2].position.x = x2;
	vertexData[2].position.y = y2;
	vertexData[2].position.z = 0.0f;

	vertexData[3].position.x = x3;
	vertexData[3].position.y = y3;
	vertexData[3].position.z = 0.0f;

	vertexData[0].texCoord.x = 0;
	vertexData[0].texCoord.y = 0;
	vertexData[1].texCoord.x = 0;
	vertexData[1].texCoord.y = 0;
	vertexData[2].texCoord.x = 0;
	vertexData[2].texCoord.y = 0;
	vertexData[3].texCoord.x = 0;
	vertexData[3].texCoord.y = 0;

	//頂点カラー
	vertexData[0].color = data->GetColor();
	vertexData[1].color = data->GetColor();
	vertexData[2].color = data->GetColor();
	vertexData[3].color = data->GetColor();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//頂点データ更新
	context->UpdateSubresource(
		vertexBuffer.Get(), 0, NULL, vertexData, 0, 0);

	context->VSSetConstantBuffers(0, 1, vertexBuffer.GetAddressOf());

	//https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetprimitivetopology
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-rssetstate
	context->RSSetState(rasterizerState.Get());

	context->PSSetShaderResources(0, 1, texture->GetSRView());

	// https://docs.microsoft.com/ja-jp/previous-versions/direct-x/ee419717(v=vs.85)?redirectedfrom=MSDN
	context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

	context->OMSetDepthStencilState(depthStencilState.Get(), 1);


	context->DrawIndexed(4,0, 0);
}
