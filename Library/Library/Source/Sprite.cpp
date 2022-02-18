#include "../Header/DirectXSystem.h"
#include "../Header/Sprite.h"


//
//Sprite::Sprite() :texture(nullptr)
//{
//	VERTEX v[] = {
//		DirectX::XMFLOAT3(-0.5f, 0.5f,0), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(0,0), DirectX::XMFLOAT4(1,1,1,1), //����
//		DirectX::XMFLOAT3(0.5f, 0.5f,0),  DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,0), DirectX::XMFLOAT4(1,1,1,1), //�E��
//		DirectX::XMFLOAT3(-0.5f,-0.5f,0), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(0,1), DirectX::XMFLOAT4(1,1,1,1), //����
//		DirectX::XMFLOAT3(0.5f,-0.5f,0),  DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,1), DirectX::XMFLOAT4(1,1,1,1), //�E��
//	};
//
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd, sizeof(bd));
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(v);
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//
//	D3D11_SUBRESOURCE_DATA res;
//	ZeroMemory(&res, sizeof(res));
//	res.pSysMem = v;
//	
//	static ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();
//	device->CreateBuffer(&bd, &res, vertexBuffer.GetAddressOf());
//
//	// �f�v�X�X�e���V���X�e�[�g
//	D3D11_DEPTH_STENCIL_DESC depth_Stencil_Desc;
//	ZeroMemory(&depth_Stencil_Desc, sizeof(depth_Stencil_Desc));
//	depth_Stencil_Desc.DepthEnable = FALSE;
//	device->CreateDepthStencilState(&depth_Stencil_Desc, depthStencilState.GetAddressOf());
//}
//
//Sprite::Sprite(const std::string fileName) :texture(nullptr)
//{
//	VERTEX v[] = {
//		DirectX::XMFLOAT3(-0.5f, 0.5f,0), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(0,0), DirectX::XMFLOAT4(1,1,1,1), //����
//		DirectX::XMFLOAT3(0.5f, 0.5f,0),  DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,0), DirectX::XMFLOAT4(1,1,1,1), //�E��
//		DirectX::XMFLOAT3(-0.5f,-0.5f,0), DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(0,1), DirectX::XMFLOAT4(1,1,1,1), //����
//		DirectX::XMFLOAT3(0.5f,-0.5f,0),  DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT2(1,1), DirectX::XMFLOAT4(1,1,1,1), //�E��
//	};
//
//	//	���_�o�b�t�@�쐬
//	D3D11_BUFFER_DESC bd;
//	ZeroMemory(&bd, sizeof(bd)); // �S���O
//	bd.Usage = D3D11_USAGE_DEFAULT;
//	bd.ByteWidth = sizeof(v);
//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//
//	D3D11_SUBRESOURCE_DATA res;
//	ZeroMemory(&res, sizeof(res));
//	res.pSysMem = v;
//
//	static ID3D11Device* device = StartingOver::DirectX11Manager::GetDevice();
//	device->CreateBuffer(&bd, &res, vertexBuffer.GetAddressOf());
//	const std::string filePath = "././Assets/Sprites/" + fileName;
//	//	�e�N�X�`���ǂݍ���
//	if (!filePath.empty()) {
//		texture = std::make_shared<Texture>();
//		texture->Load(filePath);
//	}
//
//
//	//�f�v�X�X�e���V���X�e�[�g
//	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
//	ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
//	depth_stencil_desc.DepthEnable = FALSE;
//	device->CreateDepthStencilState(&depth_stencil_desc, depthStencilState.GetAddressOf());
//
//}
//
//
//void Sprite::Render(Shader* shader,const Matrix _v = Matrix(),const Matrix _p = Matrix())
//{
//	shader->Activate();
//	//���_�f�[�^�ݒ�
//	VERTEX data[4];
//
//	float dx = texPos.x;
//	float dy = texPos.y;
//	float dw = size.x;
//	float dh = size.y;
//
//	data[0].Pos.x = dx;
//	data[0].Pos.y = dy;
//	data[0].Pos.z = 0.0f;
//
//	data[1].Pos.x = dx + dw;
//	data[1].Pos.y = dy;
//	data[1].Pos.z = 0.0f;
//
//	data[2].Pos.x = dx;
//	data[2].Pos.y = dy + dh;
//	data[2].Pos.z = 0.0f;
//
//	data[3].Pos.x = dx + dw;
//	data[3].Pos.y = dy + dh;
//	data[3].Pos.z = 0.0f;
//
//
//	// ���K���f�o�C�X���W�n
//	for (int i = 0; i < 4; i++) {
//		data[i].Pos.x = 2.0f * data[i].Pos.x / StartingOver::DirectX11Manager::GetScreenWidth() - 1.0f;
//		data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / StartingOver::DirectX11Manager::GetScreenHeight();
//		data[i].Pos.z = 0.0f;
//	}
//
//	float sx = texPos.x;
//	float sy = texPos.y;
//	float sw = texSize.x;
//	float sh = texSize.y;
//
//	//�e�N�X�`�����W�ݒ�
//	data[0].Tex.x = sx;
//	data[0].Tex.y = sy;
//	data[1].Tex.x = sx + sw;
//	data[1].Tex.y = sy;
//	data[2].Tex.x = sx;
//	data[2].Tex.y = sy + sh;
//	data[3].Tex.x = sx + sw;
//	data[3].Tex.y = sy + sh;
//
//	//UV���W
//	for (int i = 0; i < 4; i++) {
//		data[i].Tex.x = data[i].Tex.x / texture->GetWidth();
//		data[i].Tex.y = data[i].Tex.y / texture->GetHeight();
//	}
//	//���_�J���[
//	data[0].Colour = colour;
//	data[1].Colour = colour;
//	data[2].Colour = colour;
//	data[3].Colour = colour;
//	//�@��
//	data[0].Normal = DirectX::XMFLOAT3(0, 0, 1);
//	data[1].Normal = DirectX::XMFLOAT3(0, 0, 1);
//	data[2].Normal = DirectX::XMFLOAT3(0, 0, 1);
//	data[3].Normal = DirectX::XMFLOAT3(0, 0, 1);
//
//	static ID3D11DeviceContext* context = StartingOver::DirectX11Manager::GetDeviceContext();
//	//���_�f�[�^�X�V
//	context->UpdateSubresource(
//		vertexBuffer.Get(), 0, NULL, data, 0, 0);
//
//	//	���_�o�b�t�@�̎w��
//	UINT stride = sizeof(VERTEX);
//	UINT offset = 0;
//	context->IASetVertexBuffers(
//		0, 1, vertexBuffer.GetAddressOf(), // �X���b�g, ��, �o�b�t�@
//		&stride,		// �P���_�̃T�C�Y
//		&offset			// �J�n�ʒu
//	);
//	context->IASetPrimitiveTopology(
//		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
//	);
//	//�e�N�X�`���̐ݒ�
//	if (texture) texture->Set();
//
//	context->Draw(4, 0);
//}

//void Sprite::Render(Shader* shader,
//	Texture* tex,
//	float dx, float dy, float dw, float dh,
//	float sx, float sy, float sw, float sh,
//	float alpha
//)
//{
//	shader->Activate();
//	//���_�f�[�^�ݒ�
//	VERTEX data[4];
//
//	data[0].Pos.x = dx;
//	data[0].Pos.y = dy;
//	data[0].Pos.z = 0.0f;
//
//	data[1].Pos.x = dx + dw;
//	data[1].Pos.y = dy;
//	data[1].Pos.z = 0.0f;
//
//	data[2].Pos.x = dx;
//	data[2].Pos.y = dy + dh;
//	data[2].Pos.z = 0.0f;
//
//	data[3].Pos.x = dx + dw;
//	data[3].Pos.y = dy + dh;
//	data[3].Pos.z = 0.0f;
//
//
//	// ���K���f�o�C�X���W�n
//	for (int i = 0; i < 4; i++) {
//		data[i].Pos.x = 2.0f * data[i].Pos.x / StartingOver::DirectX11Manager::GetScreenWidth() - 1.0f;
//		data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / StartingOver::DirectX11Manager::GetScreenHeight();
//		data[i].Pos.z = 0.0f;
//	}
//
//
//	//�e�N�X�`�����W�ݒ�
//	data[0].Tex.x = sx;
//	data[0].Tex.y = sy;
//	data[1].Tex.x = sx + sw;
//	data[1].Tex.y = sy;
//	data[2].Tex.x = sx;
//	data[2].Tex.y = sy + sh;
//	data[3].Tex.x = sx + sw;
//	data[3].Tex.y = sy + sh;
//
//	//UV���W
//	for (int i = 0; i < 4; i++) {
//		data[i].Tex.x = data[i].Tex.x / tex->GetWidth();
//		data[i].Tex.y = data[i].Tex.y / tex->GetHeight();
//	}
//	//���_�J���[
//	data[0].Colour = DirectX::XMFLOAT4(1, 1, 1, alpha);
//	data[1].Colour = DirectX::XMFLOAT4(1, 1, 1, alpha);
//	data[2].Colour = DirectX::XMFLOAT4(1, 1, 1, alpha);
//	data[3].Colour = DirectX::XMFLOAT4(1, 1, 1, alpha);
//	//�@��
//	data[0].Normal = DirectX::XMFLOAT3(0, 0, 1);
//	data[1].Normal = DirectX::XMFLOAT3(0, 0, 1);
//	data[2].Normal = DirectX::XMFLOAT3(0, 0, 1);
//	data[3].Normal = DirectX::XMFLOAT3(0, 0, 1);
//
//	//���_�f�[�^�X�V
//	StartingOver::DirectX11Manager::GetDeviceContext()->UpdateSubresource(
//		vertexBuffer.Get(), 0, NULL, data, 0, 0);
//
//	//	���_�o�b�t�@�̎w��
//	UINT stride = sizeof(VERTEX);
//	UINT offset = 0;
//	StartingOver::DirectX11Manager::GetDeviceContext()->IASetVertexBuffers(
//		0, 1, vertexBuffer.GetAddressOf(), // �X���b�g, ��, �o�b�t�@
//		&stride,		// �P���_�̃T�C�Y
//		&offset			// �J�n�ʒu
//	);
//	StartingOver::DirectX11Manager::GetDeviceContext()->IASetPrimitiveTopology(
//		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
//	);
//	StartingOver::DirectX11Manager::GetDeviceContext()->OMSetDepthStencilState(depthStencilState.Get(), 1);
//
//	//�e�N�X�`���̐ݒ�
//	if (tex) tex->Set();
//
//	StartingOver::DirectX11Manager::GetDeviceContext()->Draw(4, 0);
//}