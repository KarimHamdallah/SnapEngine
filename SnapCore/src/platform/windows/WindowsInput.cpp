#include "SnapPCH.h"
#include <Snap/Core/Input.h>
#include <Snap/Core/Application.h>
#include <GLFW/glfw3.h>

namespace SnapEngine
{
	//Input* Input::s_Instance = new WindowsInput();

	bool Input::IsKeyPressed(Key KeyCode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int KeyState = glfwGetKey(window, (int)KeyCode);

		return KeyState == GLFW_PRESS || KeyState == GLFW_REPEAT;
	}

	bool Input::IsKeyReleased(Key KeyCode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int KeyState = glfwGetKey(window, (int)KeyCode);

		return KeyState == GLFW_RELEASE;
	}

	bool Input::IsKeyDown(Key KeyCode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int KeyState = glfwGetKey(window, (int)KeyCode);

		return KeyState == GLFW_PRESS && KeyState != GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseButton Button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int ButtonState = glfwGetMouseButton(window, (int)Button);

		return ButtonState == GLFW_PRESS || ButtonState == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonReleased(MouseButton Button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int ButtonState = glfwGetMouseButton(window, (int)Button);

		return ButtonState == GLFW_RELEASE;
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePos();
		return (float)x;
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePos();
		return (float)y;
	}

	std::pair<float, float> Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return std::pair<float, float>((float)x, float(y));
	}
}