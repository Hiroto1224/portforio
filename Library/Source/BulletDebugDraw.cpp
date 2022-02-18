
#include "../Header/BulletDebugDraw.h"

using namespace DirectX;

namespace {

	//コンパイル済みシェーダコード
#include "../Shader/btdbg_psh.hpp"
#include "../Shader/btdbg_vsh.hpp"

//シェーダ定数バッファ
	struct ConstBuff
	{
		XMMATRIX mtxProj;
		XMMATRIX mtxView;
		XMMATRIX mtxWorld;
		XMVECTOR Diffuse;
	};

}

IBulletDebugDrawDX11::IBulletDebugDrawDX11()
{
	pScene = (RenderScene*)_aligned_malloc(sizeof(RenderScene), __alignof(RenderScene));
	pScene->mtxProj = pScene->mtxView = XMMatrixIdentity();
}

IBulletDebugDrawDX11::~IBulletDebugDrawDX11()
{
	_aligned_free(pScene);
}

HRESULT IBulletDebugDrawDX11::Setup(ID3D11Device* dev, ID3D11DeviceContext* ctx, bool deferred)
{
	if (!dev || !ctx)return E_FAIL;
	HRESULT hr;
	bDeferred = deferred;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vsh;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> psh;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> ilayout;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rs;
	Microsoft::WRL::ComPtr<ID3D11BlendState> bd;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> ds_on, ds_off;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cb;

	// 頂点シェーダ作成
	hr = dev->CreateVertexShader(btdbg_vsh, sizeof(btdbg_vsh), NULL, &vsh);
	if (FAILED(hr)) { return hr; }

	// ピクセルシェーダ作成
	hr = dev->CreatePixelShader(btdbg_psh, sizeof(btdbg_psh), NULL, &psh);
	if (FAILED(hr)) { return hr; }

	// 入力レイアウト定義
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT elem_num = ARRAYSIZE(layout);

	// 入力レイアウト作成
	hr = dev->CreateInputLayout(layout, elem_num, btdbg_vsh, sizeof(btdbg_vsh), &ilayout);
	if (FAILED(hr)) { return hr; }

	//ステート設定
	// ラスタライザステート
	D3D11_RASTERIZER_DESC rsdesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	rsdesc.AntialiasedLineEnable = TRUE;
	rsdesc.MultisampleEnable = FALSE;
	hr = dev->CreateRasterizerState(&rsdesc, &rs);
	if (FAILED(hr)) { return hr; }

	// ブレンドステート
	D3D11_BLEND_DESC bddesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	bddesc.RenderTarget[0].BlendEnable = TRUE;
	bddesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bddesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bddesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bddesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bddesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bddesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	hr = dev->CreateBlendState(&bddesc, &bd);
	if (FAILED(hr)) { return hr; }

	// デプスステンシルステート
	D3D11_DEPTH_STENCIL_DESC dsdesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	dsdesc.DepthEnable = TRUE;
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = dev->CreateDepthStencilState(&dsdesc, &ds_on);
	if (FAILED(hr)) { return hr; }
	dsdesc.DepthEnable = FALSE;
	dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = dev->CreateDepthStencilState(&dsdesc, &ds_off);


	// 定数バッファ
	ConstBuff cbuff;
	cbuff.mtxProj = cbuff.mtxView = cbuff.mtxWorld = XMMatrixIdentity();
	D3D11_BUFFER_DESC bfdesc;
	ZeroMemory(&bfdesc, sizeof(bfdesc));
	bfdesc.Usage = D3D11_USAGE_DEFAULT;
	bfdesc.ByteWidth = sizeof(ConstBuff);
	bfdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bfdesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA initdat;
	ZeroMemory(&initdat, sizeof(initdat));
	initdat.pSysMem = &cbuff;
	hr = dev->CreateBuffer(&bfdesc, &initdat, &cb);
	if (FAILED(hr)) {
		return hr;
	}

	pVShader = vsh;
	pPShader = psh;
	pLayout = ilayout;
	pRsState = rs;
	pBdState = bd;
	pDsTestOn = ds_on;
	pDsTestOff = ds_off;
	pCBuff = cb;
	pDev = dev;
	pCtx = ctx;
	aLine.clear();

	return hr;
}


void IBulletDebugDrawDX11::SetProjMatrix(const Matrix& proj)
{
	pScene->mtxProj = proj;
}
void IBulletDebugDrawDX11::SetViewMatrix(const Matrix& view)
{
	pScene->mtxView = view;
}

// ライン描画実行
void IBulletDebugDrawDX11::FlushLine()
{
	if (aLine.size() == 0)return;

	//頂点データ構造体
	struct VertexData
	{
		XMFLOAT3 pos;
		XMFLOAT3 col;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> vbuff, idxbuff;

	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Line) * aLine.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = aLine.data();
	hr = pDev->CreateBuffer(&bd, &InitData, &vbuff);
	if (FAILED(hr)) {
		aLine.clear();
		return;
	}

	// 定数バッファ 変換行列（プロジェクション、ビュー）
	ConstBuff cbuff;
	XMVECTOR Eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	cbuff.mtxView = XMMatrixTranspose(pScene->mtxView);
	cbuff.mtxProj = XMMatrixTranspose(pScene->mtxProj);
	cbuff.mtxWorld = XMMatrixIdentity();
	cbuff.Diffuse = XMVectorSet(0.5f, 0.5f, 0.5f, 0.25f);
	pCtx->UpdateSubresource(pCBuff.Get(), 0, NULL, &cbuff, 0, 0);

	// 入力レイアウト
	pCtx->IASetInputLayout(pLayout.Get());

	// 頂点バッファ
	UINT stride = sizeof(VertexData);
	UINT offset = 0;
	ID3D11Buffer* vb[1] = { vbuff.Get() };
	pCtx->IASetVertexBuffers(0, 1, vb, &stride, &offset);

	// プリミティブ形状
	pCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// シェーダ
	ID3D11Buffer* cb[1] = { pCBuff.Get() };
	pCtx->VSSetConstantBuffers(0, 1, cb);
	pCtx->PSSetConstantBuffers(0, 1, cb);
	pCtx->VSSetShader(pVShader.Get(), NULL, 0);
	pCtx->GSSetShader(NULL, NULL, 0);
	pCtx->PSSetShader(pPShader.Get(), NULL, 0);

	// ステート
	FLOAT bdfct[4] = { 0,0,0,0 };
	pCtx->RSSetState(pRsState.Get());
	pCtx->OMSetBlendState(pBdState.Get(), bdfct, 0xffffffff);
	pCtx->OMSetDepthStencilState(pDsTestOff.Get(), 0);

	//ポリゴン描画
	pCtx->Draw(aLine.size() * 2, 0);

	cbuff.Diffuse = XMVectorSet(1, 1, 1, 1);
	pCtx->UpdateSubresource(pCBuff.Get(), 0, NULL, &cbuff, 0, 0);
	pCtx->OMSetDepthStencilState(pDsTestOn.Get(), 0);
	pCtx->Draw(aLine.size() * 2, 0);


	aLine.clear();
	pCtx->VSSetShader(NULL, NULL, 0);
	pCtx->GSSetShader(NULL, NULL, 0);
	pCtx->PSSetShader(NULL, NULL, 0);

}


void IBulletDebugDrawDX11::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
	// バッファにまとめて、あとから一括描画
	if (aLine.size() >= MAX_LINE)return;
	aLine.push_back(Line(from, to, fromColor, toColor));
}

void IBulletDebugDrawDX11::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	drawLine(from, to, color, color);
}

IBulletDebugDrawDX11::Vertex::Vertex(const btVector3& p, const btVector3& c)
{
	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];
	color[0] = c[0];
	color[1] = c[1];
	color[2] = c[2];
}



