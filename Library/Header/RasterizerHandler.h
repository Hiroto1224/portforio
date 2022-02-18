#pragma once
#include <map>
#include <string>
#include <d3d11.h>

#include "Rasterizer.h"
#include "MyMath.h"

/// <summary>
/// ���ꂽRasterizer�����������̃N���X
/// </summary>
class RasterizerHandler
{
public:
	RasterizerHandler() = default;
	virtual ~RasterizerHandler() = default;
	RasterizerHandler(RasterizerHandler&) = delete;
	RasterizerHandler& operator=(RasterizerHandler&) = delete;

	/// <summary>
	/// Rasterizer���󂯎��
	/// </summary>
	/// <param name="rasterizerName"> �󂯎��Rasterizer�̖��O </param>
	/// <param name="rasterizer"> �󂯎��Rasterizer </param>
	bool ReceiveRasterizer(std::string rasterizerName, const std::shared_ptr<StartingOver::Rasterizer> rasterizer);

	/// <summary>
	/// �w�肵��Rasterizer��L���ɂ���
	/// </summary>
	/// <param name="rasterizerName"> �L���ɂ���Rasterizer�̖��O </param>
	/// <param name="shaderResourceView"> FrameBuffer��ShaderResourceView </param>
	/// <param name="position"> ��n�_�̍��W </param>
	/// <param name="size"> ScreenSize </param>
	void Blit(std::string rasterizerName,ID3D11ShaderResourceView* shaderResourceView,const Vector2 position,const Vector2 size);

	/// <summary>
	/// �w�肵��Rasterizer��weak_ptr�Ŏ󂯎��
	/// </summary>
	/// <param name="rasterizerName"> �󂯎��Rasterizer�̖��O </param>
	std::weak_ptr<StartingOver::Rasterizer> GetRasterizer(std::string rasterizerName);

private:
	//�@�󂯎����Rasterizer��ۑ�
	std::map<std::string, std::shared_ptr<StartingOver::Rasterizer>> rasterizers;
};