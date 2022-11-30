#include "SnapPCH.h"
#include "Texture.h"

#include "RendererAPI.h"
#include <platform/OpenGL/OpenGLTexture.h>

namespace SnapEngine
{
	Texture2D* Texture2D::Creat(const std::string& filepath, const Textureprops& props, bool flip)
	{
		switch (RendererAPI::GetAPI())
		{
		   case RendererAPI::API::None:         SNAP_ASSERT_MSG(false, "RendererAPI::None is Currently not supported!");
		   case RendererAPI::API::OpenGL:       return new OpenGLTexture(filepath, props, flip);
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}

	Texture2D* Texture2D::Creat(uint32_t Width, uint32_t Height, const Textureprops& props, bool flip)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:         SNAP_ASSERT_MSG(false, "RendererAPI::None is Currently not supported!");
		case RendererAPI::API::OpenGL:       return new OpenGLTexture(Width, Height, props, flip);
		}

		SNAP_ASSERT_MSG(false, "RendererAPI is UNKOWN!");
		return nullptr;
	}
}