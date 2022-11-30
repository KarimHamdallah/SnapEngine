#include "SnapPCH.h"
#include "WindowsInput.h"
#include <Snap/Core/Application.h>
#include <GLFW/glfw3.h>

namespace SnapEngine
{
	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int KeyCode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int KeyState = glfwGetKey(window, KeyCode);

		return KeyState == GLFW_PRESS || KeyState == GLFW_REPEAT;
	}

	bool WindowsInput::IsKeyReleasedImpl(int KeyCode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int KeyState = glfwGetKey(window, KeyCode);

		return KeyState == GLFW_RELEASE;
	}

	bool WindowsInput::IsKeyDownImpl(int KeyCode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int KeyState = glfwGetKey(window, KeyCode);

		return KeyState == GLFW_PRESS && KeyState != GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int Button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int ButtonState = glfwGetMouseButton(window, Button);

		return ButtonState == GLFW_PRESS || ButtonState == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonReleasedImpl(int Button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		int ButtonState = glfwGetMouseButton(window, Button);

		return ButtonState == GLFW_RELEASE;
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return (float)x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePosImpl();
		return (float)y;
	}

	std::pair<float, float> WindowsInput::GetMousePosImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return std::pair<float, float>((float)x, float(y));
	}
}