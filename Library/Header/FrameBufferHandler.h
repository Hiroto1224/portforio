#pragma once
#include <map>
#include <string>

#include "FrameBuffer.h"


/// <summary>
/// 作られたFrameBufferを扱うだけのクラス
/// </summary>
class FrameBufferHandler
{
public:
	FrameBufferHandler() = default;
	virtual ~FrameBufferHandler() = default;
	FrameBufferHandler(FrameBufferHandler&) = delete;
	FrameBufferHandler& operator=(FrameBufferHandler&) = delete;

	/// <summary>
	/// FrameBufferを受け取る
	/// </summary>
	/// <param name="bufferName"> 受け取るFrameBufferの名前 </param>
	/// <param name="frameBuffer"> 受け取るFrameBuffer </param>
	bool ReceiveFrameBuffer(std::string bufferName , std::shared_ptr<StartingOver::FrameBuffer> frameBuffer);

	/// <summary>
	/// 指定したFrameBufferを有効にする
	/// </summary>
	/// <param name="bufferName"> 有効にするFrameBufferの名前 </param>
	void Activate(std::string bufferName);

	/// <summary>
	/// 指定したFrameBufferを無効にする
	/// </summary>
	/// <param name="bufferName"> 無効にするFrameBufferの名前 </param>
	void InActivate(std::string bufferName);

	/// <summary>
	/// 指定したFrameBufferをweak_ptrで受け取る
	/// </summary>
	/// <param name="bufferName"> 受け取るFrameBufferの名前 </param>
	std::weak_ptr<StartingOver::FrameBuffer> GetFrameBuffer(std::string bufferName);

private:
	//　受け取ったFrameBufferを保存
	std::map<std::string,std::shared_ptr<StartingOver::FrameBuffer>> frameBuffers;
};