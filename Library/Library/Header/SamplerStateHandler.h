#pragma once
#include <memory>
#include <string>

#include "SamplerState.h"


class SamplerStateHandler
{
public:
	SamplerStateHandler() = default;
	virtual ~SamplerStateHandler() = default;
	SamplerStateHandler(SamplerStateHandler&) = delete;
	SamplerStateHandler& operator=(SamplerStateHandler&) = delete;

	/// <summary>
	/// SamplerStateを受け取る
	/// </summary>
	/// <param name="stateName"> 受け取るSamplerStateの名前 </param>
	/// <param name="samplerState"> 受け取るSamplerState </param>
	/// <returns></returns>
	bool ReceiveSamplerState(std::string stateName, const std::shared_ptr<StartingOver::SamplerState> samplerState);

	/// <summary>
	/// 指定したSamplerStateを有効にする
	/// </summary>
	/// <param name="stateName"> 有効にするSamplerStateの名前 </param>
	/// <param name="slot"></param>
	/// <param name="setVS"></param>
	void Activate(std::string stateName, int slot, bool setVS);

	/// <summary>
	/// 指定したSamplerStateを無効にする
	/// </summary>
	/// <param name="stateName"> 無効にするSamplerStateの名前 </param>
	void InActivate(std::string stateName);

	/// <summary>
	/// 指定したSamplerStateをweak_ptrで受け取る
	/// </summary>
	/// <param name="stateName"> 受け取るSamplerStateの名前 </param>
	std::weak_ptr<StartingOver::SamplerState> GetSamplerState(std::string stateName);

private:
	//　受け取ったSamplerStateを保存
	std::map<std::string, std::shared_ptr<StartingOver::SamplerState>> samplerStates;

};