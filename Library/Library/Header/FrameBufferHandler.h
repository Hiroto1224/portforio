#pragma once
#include <map>
#include <string>

#include "FrameBuffer.h"


/// <summary>
/// ���ꂽFrameBuffer�����������̃N���X
/// </summary>
class FrameBufferHandler
{
public:
	FrameBufferHandler() = default;
	virtual ~FrameBufferHandler() = default;
	FrameBufferHandler(FrameBufferHandler&) = delete;
	FrameBufferHandler& operator=(FrameBufferHandler&) = delete;

	/// <summary>
	/// FrameBuffer���󂯎��
	/// </summary>
	/// <param name="bufferName"> �󂯎��FrameBuffer�̖��O </param>
	/// <param name="frameBuffer"> �󂯎��FrameBuffer </param>
	bool ReceiveFrameBuffer(std::string bufferName , std::shared_ptr<StartingOver::FrameBuffer> frameBuffer);

	/// <summary>
	/// �w�肵��FrameBuffer��L���ɂ���
	/// </summary>
	/// <param name="bufferName"> �L���ɂ���FrameBuffer�̖��O </param>
	void Activate(std::string bufferName);

	/// <summary>
	/// �w�肵��FrameBuffer�𖳌��ɂ���
	/// </summary>
	/// <param name="bufferName"> �����ɂ���FrameBuffer�̖��O </param>
	void InActivate(std::string bufferName);

	/// <summary>
	/// �w�肵��FrameBuffer��weak_ptr�Ŏ󂯎��
	/// </summary>
	/// <param name="bufferName"> �󂯎��FrameBuffer�̖��O </param>
	std::weak_ptr<StartingOver::FrameBuffer> GetFrameBuffer(std::string bufferName);

private:
	//�@�󂯎����FrameBuffer��ۑ�
	std::map<std::string,std::shared_ptr<StartingOver::FrameBuffer>> frameBuffers;
};