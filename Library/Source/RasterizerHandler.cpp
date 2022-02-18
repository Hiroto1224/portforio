#include "../Header/RasterizerHandler.h"
#include "../Header/misc.h"



bool RasterizerHandler::ReceiveRasterizer(std::string rasterizerName, const std::shared_ptr<StartingOver::Rasterizer> rasterizer)
{

	if (rasterizer == nullptr)
	{
		// Žó‚¯Žæ‚è‚ÉŽ¸”s‚µ‚½‚Æ‚«
		_ASSERT_EXPR_A(rasterizer, "Failed to receive Sampler State.");
	}

	auto it = rasterizers.find(rasterizerName);
	if (it != rasterizers.end())
	{
		//@“¯‚¶–¼‘O‚Ì•¨‚ª‘¶Ý‚µ‚Ä‚¢‚½Žž
		_ASSERT_EXPR_A(&rasterizer, "A Sampler State of that name does exist.");
	}

	rasterizers.insert(std::make_pair(rasterizerName, rasterizer));

	return true;
}

void RasterizerHandler::Blit(std::string rasterizerName, ID3D11ShaderResourceView* shaderResourceView, const Vector2 position, const Vector2 size)
{
	rasterizers.at(rasterizerName)->Blit(shaderResourceView, position, size);
}

std::weak_ptr<StartingOver::Rasterizer> RasterizerHandler::GetRasterizer(std::string rasterizerName)
{
	return std::weak_ptr<StartingOver::Rasterizer>(rasterizers.at(rasterizerName));
}

