#include "SnapPCH.h"
#include "Buffer.h"

#include "Renderer.h"
#include "platform/OpenGL/OpenGLBuffer.h"

namespace SnapEngine
{
	VertexBuffer* VertexBuffer::Creat(float* vertices, size_t size)
	{
		RendererAPI::API api = Renderer::GetAPI();
		switch (api)
		{
		    case RendererAPI::API::None:         SNAP_ASSERT_MSG(false, "RendererAPI::None is Currently not supported!");
		    case RendererAPI::API::OpenGL:       return new OpenGLVertexBuffer(vertices, size);
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}

	VertexBuffer* VertexBuffer::Creat(size_t size)
	{
		RendererAPI::API api = Renderer::GetAPI();
		switch (api)
		{
		case RendererAPI::API::None:         SNAP_ASSERT_MSG(false, "RendererAPI::None is Currently not supported!");
		case RendererAPI::API::OpenGL:       return new OpenGLVertexBuffer(size);
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Creat(uint32_t* indices, uint32_t count)
	{
		RendererAPI::API api = Renderer::GetAPI();
		switch (api)
		{
		case RendererAPI::API::None:         SNAP_ASSERT_MSG(false, "RendererAPI::None is Currently not supported!");
		case RendererAPI::API::OpenGL:       return new OpenGLIndexBuffer(indices, count);
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}
}