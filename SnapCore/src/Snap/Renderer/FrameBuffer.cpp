#include "SnapPCH.h"
#include "FrameBuffer.h"

#include <Snap/Renderer/RendererAPI.h>
#include <platform/OpenGL/OpenGLFrameBuffer.h>

namespace SnapEngine
{
	FrameBuffer* FrameBuffer::Creat(uint32_t Width, uint32_t Height)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:     SNAP_ASSERT_MSG(false, "RendererAPI::None is currently not supported!");
		case RendererAPI::API::OpenGL:   return new OpenGLFrameBuffer(Width, Height);
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}
}