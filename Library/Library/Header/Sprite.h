#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include "MyMath.h"
#include "Shader.h"
#include "Texture.h"
#include "RenderObject.h"


class Sprite : public RenderObject
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	std::shared_ptr<Texture> texture;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	Vector2 size = { 100,100 };
	Vector2 texPos = { 0,0 };
	Vector2 texSize = { 100,100 };
	Vector4 colour = { 1,1,1,1 };

public:
	Sprite();
	Sprite(const std::string filename);
	~Sprite(){};

	void SetSize(Vector2 _v) { size = _v; }
	void SetTexPos(Vector2 _v) { texPos = _v; }
	void SetTexSize(Vector2 _v) { texSize = _v; }
	void SetColour(Vector4 _colour) { colour = _colour; }

	//void Render(Shader* shader, const Matrix _v , const Matrix _p) override;

private:


	ID3D11RenderTargetView* GetRenderTarget() { return texture->GetRenderTarget(); }

	void SetTexture(u_int slot) { texture->Set(slot); }
};