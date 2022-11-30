#pragma once

#include <Snap/Renderer/GraphicsContext.h>

struct GLFWwindow;

namespace SnapEngine
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* WindowHandel);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* WindowHandel = nullptr;
	};
}