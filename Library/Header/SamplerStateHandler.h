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
	/// SamplerState���󂯎��
	/// </summary>
	/// <param name="stateName"> �󂯎��SamplerState�̖��O </param>
	/// <param name="samplerState"> �󂯎��SamplerState </param>
	/// <returns></returns>
	bool ReceiveSamplerState(std::string stateName, const std::shared_ptr<StartingOver::SamplerState> samplerState);

	/// <summary>
	/// �w�肵��SamplerState��L���ɂ���
	/// </summary>
	/// <param name="stateName"> �L���ɂ���SamplerState�̖��O </param>
	/// <param name="slot"></param>
	/// <param name="setVS"></param>
	void Activate(std::string stateName, int slot, bool setVS);

	/// <summary>
	/// �w�肵��SamplerState�𖳌��ɂ���
	/// </summary>
	/// <param name="stateName"> �����ɂ���SamplerState�̖��O </param>
	void InActivate(std::string stateName);

	/// <summary>
	/// �w�肵��SamplerState��weak_ptr�Ŏ󂯎��
	/// </summary>
	/// <param name="stateName"> �󂯎��SamplerState�̖��O </param>
	std::weak_ptr<StartingOver::SamplerState> GetSamplerState(std::string stateName);

private:
	//�@�󂯎����SamplerState��ۑ�
	std::map<std::string, std::shared_ptr<StartingOver::SamplerState>> samplerStates;

};