#pragma once
#include <btBulletDynamicsCommon.h>
#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <vector>

#include "MyMath.h"


// Todo âº
class IBulletDebugDrawDX11 : public btIDebugDraw
{
public:
	IBulletDebugDrawDX11();
	virtual ~IBulletDebugDrawDX11();

	// deferred = true DirectX11ÇÃï`âÊñΩóﬂÇíxâÑÇ≥ÇπÇÈ DeferredRender()Ç≈ï`âÊé¿çs
	//            false debugDrawWorld()éûÇ…ï`âÊé¿çs
	HRESULT Setup(ID3D11Device* dev, ID3D11DeviceContext* ctx, bool deferred = true);

	void SetProjMatrix(const Matrix& proj);
	void SetViewMatrix(const Matrix& view);

public:
	// ï`âÊñΩóﬂÇíxâÑ
	void DeferredRender()
	{
		if (bDeferred) {
			if (bFlush)FlushLine();
			bFlush = false;
		}
	}

	virtual void	setDebugMode(int debugMode) { bitDebugMode = debugMode; }
	virtual int		getDebugMode() const { return bitDebugMode; }




	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	virtual void flushLines()
	{
		if (!bDeferred) {
			FlushLine();
		}
		else {
			bFlush = true;
		}
	}
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		drawLine(PointOnB, PointOnB + normalOnB * distance, color);
		btVector3 ncolor(0, 0, 0);
		drawLine(PointOnB, PointOnB + normalOnB * 0.01f, ncolor);
	}

	//ñ¢ëŒâû
	virtual void	reportErrorWarning(const char* warningString) {}
	virtual void	draw3dText(const btVector3& location, const char* textString) {}

private:
	//ÉâÉCÉìï`âÊé¿çs
	void FlushLine();

	int bitDebugMode = 0;
	bool bDeferred = false;
	bool bFlush = false;

	Microsoft::WRL::ComPtr<ID3D11Device> pDev;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pCtx;

	struct RenderScene
	{
		Matrix mtxProj;
		Matrix mtxView;
	};
	RenderScene* pScene;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pCBuff;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pLayout;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRsState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBdState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDsTestOn;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDsTestOff;

	struct Vertex
	{
		Vertex() {}
		Vertex(const btVector3& p, const btVector3& c);
		FLOAT pos[3];
		FLOAT color[3];
	};
	struct Line
	{
		Line() {}
		Line(const Vertex& f, const Vertex& t) : from(f), to(t) {}
		Line(const btVector3& f, const btVector3& t, const btVector3& fc, const btVector3& tc)
			: from(f, fc), to(t, tc) {}

		Vertex from, to;
	};
	static const size_t MAX_LINE = 30000;
	std::vector<Line> aLine;
};




