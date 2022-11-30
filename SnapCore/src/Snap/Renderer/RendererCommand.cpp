#include "SnapPCH.h"
#include "RendererCommand.h"

#include <platform/OpenGL/OpenGLRendererAPI.h>

namespace SnapEngine
{
	RendererAPI* RendererCommand::s_RendererAPI = new OpenGLRendererAPI();
}