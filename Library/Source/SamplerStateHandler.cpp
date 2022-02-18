#include "../Header/SamplerStateHandler.h"
#include "../Header/misc.h"



bool SamplerStateHandler::ReceiveSamplerState(std::string stateName, const std::shared_ptr<StartingOver::SamplerState> samplerState)
{

	if (samplerState == nullptr)
	{
		// Žó‚¯Žæ‚è‚ÉŽ¸”s‚µ‚½‚Æ‚«
		_ASSERT_EXPR_A(samplerState, "Failed to receive Sampler State.");
	}

	auto it = samplerStates.find(stateName);
	if (it != samplerStates.end())
	{
		//@“¯‚¶–¼‘O‚Ì•¨‚ª‘¶Ý‚µ‚Ä‚¢‚½Žž
		_ASSERT_EXPR_A(&samplerState, "A Sampler State of that name does exist.");
	}

	samplerStates.insert(std::make_pair(stateName, samplerState));

	return true;
}

void SamplerStateHandler::Activate(const std::string stateName, const int slot, const bool setVS)
{
	samplerStates.at(stateName)->Activate(slot,setVS);
}

void SamplerStateHandler::InActivate(const std::string stateName)
{
	samplerStates.at(stateName)->InActivate();
}

std::weak_ptr<StartingOver::SamplerState> SamplerStateHandler::GetSamplerState(const std::string stateName)
{
	return std::weak_ptr<StartingOver::SamplerState>(samplerStates.at(stateName));
}

