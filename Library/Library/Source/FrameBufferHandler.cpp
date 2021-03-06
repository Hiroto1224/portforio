#include "../Header/FrameBufferHandler.h"
#include "../Header/misc.h"
#include <map>
#include <iostream>


bool FrameBufferHandler::ReceiveFrameBuffer(std::string bufferName,const std::shared_ptr<StartingOver::FrameBuffer> frameBuffer)
{

	if (frameBuffer == nullptr)
	{
		// 受け取りに失敗したとき
		_ASSERT_EXPR_A(frameBuffer, "Failed to receive Frame Buffer.");
	}

	if(const auto iterator = frameBuffers.find(bufferName); iterator != frameBuffers.end())
	{
		//　同じ名前の物が存在していた時
		_ASSERT_EXPR_A(&frameBuffer, "A Frame Buffer of that name does exist.");
	}
	
	frameBuffers.insert(std::make_pair(bufferName, frameBuffer));

	return true;
}

void FrameBufferHandler::Activate(const std::string bufferName)
{
	frameBuffers.at(bufferName)->Clear();
	frameBuffers.at(bufferName)->Activate();
}

void FrameBufferHandler::InActivate(const std::string bufferName)
{
	frameBuffers.at(bufferName)->InActivate();
}

std::weak_ptr<StartingOver::FrameBuffer> FrameBufferHandler::GetFrameBuffer(const std::string bufferName)
{
	return std::weak_ptr<StartingOver::FrameBuffer>(frameBuffers.at(bufferName));
}

