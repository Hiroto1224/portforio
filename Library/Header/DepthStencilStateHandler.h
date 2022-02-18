#pragma once
#include <map>
#include <string>

#include "DepthStencilState.h"


/// <summary>
/// 作られたDepthStencilStateを扱うだけのクラス
/// </summary>
class DepthStencilStateHandler
{
public:
	DepthStencilStateHandler() = default;
	virtual ~DepthStencilStateHandler() = default;
	DepthStencilStateHandler(DepthStencilStateHandler&) = delete;
	DepthStencilStateHandler& operator=(DepthStencilStateHandler&) = delete;

	/// <summary>
	/// DepthStencilStateを受け取る
	/// </summary>
	/// <param name="stateName"> 受け取るDepthStencilStateの名前 </param>
	/// <param name="depthStencilState"> 受け取るDepthStencilState </param>
	bool ReceiveDepthStencilState(std::string stateName, const std::shared_ptr<StartingOver::DepthStencilState> depthStencilState);

	/// <summary>
	/// 指定したDepthStencilStateを有効にする
	/// </summary>
	/// <param name="stateName"> 有効にするDepthStencilStateの名前 </param>
	void Activate(std::string stateName);

	/// <summary>
	/// 指定したDepthStencilStateを無効にする
	/// </summary>
	/// <param name="stateName"> 無効にするDepthStencilStateの名前 </param>
	void InActivate(std::string stateName);

	/// <summary>
	/// 指定したDepthStencilStateをweak_ptrで受け取る
	/// </summary>
	/// <param name="stateName"> 受け取るDepthStencilStateの名前 </param>
	std::weak_ptr<StartingOver::DepthStencilState> GetDepthStencilState(std::string stateName);

private:
	//　受け取ったDepthStencilStateを保存
	std::map<std::string, std::shared_ptr<StartingOver::DepthStencilState>> depthStencilStates;
}; 