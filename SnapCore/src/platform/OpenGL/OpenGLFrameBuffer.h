#pragma once
#include <cstdint>
#include <Snap/Renderer/FrameBuffer.h>

namespace SnapEngine
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(uint32_t Width, uint32_t Height);
		virtual ~OpenGLFrameBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;
	};
}