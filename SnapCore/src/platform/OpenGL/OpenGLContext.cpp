#include "SnapPCH.h"
#include "OpenGLContext.h"
#include <Snap/Core/asserts.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace SnapEngine
{
	OpenGLContext::OpenGLContext(GLFWwindow* WindowHandel)
		: WindowHandel(WindowHandel)
	{
		SNAP_ASSERT_MSG(this->WindowHandel, "Context WindowHandel is null!");
	}
	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(WindowHandel);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		SNAP_ASSERT_MSG(status, "Failed To Initialize GLAD!");
	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(WindowHandel);
	}
}