#include "SnapPCH.h"
#include "VertexArray.h"

#include "Snap/Core/asserts.h"
#include "Renderer.h"
#include "platform/OpenGL/OpenGLVertexArray.h"

namespace SnapEngine
{
	VertexArray* VertexArray::Creat()
	{
		RendererAPI::API api = Renderer::GetAPI();
		switch (api)
		{
		case RendererAPI::API::None:         SNAP_ASSERT_MSG(false, "RendererAPI::None is Currently not supported!");
		case RendererAPI::API::OpenGL:       return new OpenGLVertexArray();
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}
}