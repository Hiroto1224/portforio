#pragma once
#include <map>
#include <string>
#include <d3d11.h>

#include "Rasterizer.h"
#include "MyMath.h"

/// <summary>
/// 作られたRasterizerを扱うだけのクラス
/// </summary>
class RasterizerHandler
{
public:
	RasterizerHandler() = default;
	virtual ~RasterizerHandler() = default;
	RasterizerHandler(RasterizerHandler&) = delete;
	RasterizerHandler& operator=(RasterizerHandler&) = delete;

	/// <summary>
	/// Rasterizerを受け取る
	/// </summary>
	/// <param name="rasterizerName"> 受け取るRasterizerの名前 </param>
	/// <param name="rasterizer"> 受け取るRasterizer </param>
	bool ReceiveRasterizer(std::string rasterizerName, const std::shared_ptr<StartingOver::Rasterizer> rasterizer);

	/// <summary>
	/// 指定したRasterizerを有効にする
	/// </summary>
	/// <param name="rasterizerName"> 有効にするRasterizerの名前 </param>
	/// <param name="shaderResourceView"> FrameBufferのShaderResourceView </param>
	/// <param name="position"> 基準地点の座標 </param>
	/// <param name="size"> ScreenSize </param>
	void Blit(std::string rasterizerName,ID3D11ShaderResourceView* shaderResourceView,const Vector2 position,const Vector2 size);

	/// <summary>
	/// 指定したRasterizerをweak_ptrで受け取る
	/// </summary>
	/// <param name="rasterizerName"> 受け取るRasterizerの名前 </param>
	std::weak_ptr<StartingOver::Rasterizer> GetRasterizer(std::string rasterizerName);

private:
	//　受け取ったRasterizerを保存
	std::map<std::string, std::shared_ptr<StartingOver::Rasterizer>> rasterizers;
};