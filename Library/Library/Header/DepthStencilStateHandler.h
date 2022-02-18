#pragma once
#include <map>
#include <string>

#include "DepthStencilState.h"


/// <summary>
/// ���ꂽDepthStencilState�����������̃N���X
/// </summary>
class DepthStencilStateHandler
{
public:
	DepthStencilStateHandler() = default;
	virtual ~DepthStencilStateHandler() = default;
	DepthStencilStateHandler(DepthStencilStateHandler&) = delete;
	DepthStencilStateHandler& operator=(DepthStencilStateHandler&) = delete;

	/// <summary>
	/// DepthStencilState���󂯎��
	/// </summary>
	/// <param name="stateName"> �󂯎��DepthStencilState�̖��O </param>
	/// <param name="depthStencilState"> �󂯎��DepthStencilState </param>
	bool ReceiveDepthStencilState(std::string stateName, const std::shared_ptr<StartingOver::DepthStencilState> depthStencilState);

	/// <summary>
	/// �w�肵��DepthStencilState��L���ɂ���
	/// </summary>
	/// <param name="stateName"> �L���ɂ���DepthStencilState�̖��O </param>
	void Activate(std::string stateName);

	/// <summary>
	/// �w�肵��DepthStencilState�𖳌��ɂ���
	/// </summary>
	/// <param name="stateName"> �����ɂ���DepthStencilState�̖��O </param>
	void InActivate(std::string stateName);

	/// <summary>
	/// �w�肵��DepthStencilState��weak_ptr�Ŏ󂯎��
	/// </summary>
	/// <param name="stateName"> �󂯎��DepthStencilState�̖��O </param>
	std::weak_ptr<StartingOver::DepthStencilState> GetDepthStencilState(std::string stateName);

private:
	//�@�󂯎����DepthStencilState��ۑ�
	std::map<std::string, std::shared_ptr<StartingOver::DepthStencilState>> depthStencilStates;
}; 