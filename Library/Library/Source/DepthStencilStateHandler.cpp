#include "../Header/DepthStencilStateHandler.h"
#include "../Header/misc.h"



bool DepthStencilStateHandler::ReceiveDepthStencilState(std::string stateName, const std::shared_ptr<StartingOver::DepthStencilState> depthStencilState)
{

	if (depthStencilState == nullptr)
	{
		// Žó‚¯Žæ‚è‚ÉŽ¸”s‚µ‚½‚Æ‚«
		_ASSERT_EXPR_A(depthStencilState, "Failed to receive Depth Stencil State.");
	}

	auto it = depthStencilStates.find(stateName);
	if (it != depthStencilStates.end())
	{
		//@“¯‚¶–¼‘O‚Ì•¨‚ª‘¶Ý‚µ‚Ä‚¢‚½Žž
		_ASSERT_EXPR_A(&depthStencilState, "A Depth Stencil Stateof that name does exist.");
	}

	depthStencilStates.insert(std::make_pair(stateName, depthStencilState));

	return true;
}

void DepthStencilStateHandler::Activate(const std::string stateName)
{
	depthStencilStates.at(stateName)->Activate();
}

void DepthStencilStateHandler::InActivate(const std::string stateName)
{
	depthStencilStates.at(stateName)->InActivate();
}

std::weak_ptr<StartingOver::DepthStencilState> DepthStencilStateHandler::GetDepthStencilState(const std::string stateName)
{
	return std::weak_ptr<StartingOver::DepthStencilState>(depthStencilStates.at(stateName));
}

