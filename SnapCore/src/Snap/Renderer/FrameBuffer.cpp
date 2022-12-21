#include "SnapPCH.h"
#include "FrameBuffer.h"

#include <Snap/Renderer/RendererAPI.h>
#include <platform/OpenGL/OpenGLFrameBuffer.h>

namespace SnapEngine
{
	FrameBuffer* FrameBuffer::Creat(const FrameBufferSpecifications& specs)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:     SNAP_ASSERT_MSG(false, "RendererAPI::None is currently not supported!");
		case RendererAPI::API::OpenGL:   return new OpenGLFrameBuffer(specs);
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}
}