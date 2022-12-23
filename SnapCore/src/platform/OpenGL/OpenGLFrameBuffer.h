#pragma once
#include <cstdint>
#include <Snap/Renderer/FrameBuffer.h>

namespace SnapEngine
{
	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(const FrameBufferSpecifications& specs);
		virtual ~OpenGLFrameBuffer();

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual void Resize(uint32_t Width, uint32_t Height) override;
		virtual int ReadPixel(uint32_t AttachmentIndex, uint32_t x, uint32_t y) override;
		virtual void ClearAttachment(uint32_t AttachmentIndex, int value) override;

	private:
		void Invalidate();
	};
}