#include "SnapPCH.h"
#include "WindowsWindow.h"
#include <Snap/Core/asserts.h>
#include <Snap/Events/WindowEvent.h>
#include <Snap/Events/KeyEvent.h>
#include <Snap/Events/MouseEvent.h>
#include <platform/OpenGL/OpenGLContext.h>
#include <glad/glad.h>

namespace SnapEngine
{
	static bool m_GLFWInitialized = false;

	IWindow* IWindow::Creat(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
		: m_NativeWindow(nullptr)
	{
		init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::init(const WindowProps& props)
	{
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title = props.Title;
		m_Data.IsVsync = props.IsVsync;

		if (!m_GLFWInitialized)
		{
			// TODO:: glfw terminate on system shutdown
			int success = glfwInit();
			SNAP_ASSERT_MSG(success, "Failed To Initialize GLFW!");

			m_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		// Creat Window
		m_NativeWindow = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);

		// Creat Context
		m_Context = std::make_unique<OpenGLContext>(m_NativeWindow);
		m_Context->Init();

		glfwSetWindowUserPointer(m_NativeWindow, &m_Data);
		SetVsync(true);

		// CallBack Functions
		glfwSetWindowSizeCallback(m_NativeWindow,
			[](GLFWwindow* window, int width, int height)
			{
				WindowData& WinData = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowResizeEvent e(width, height);
				WinData.Width = width;
				WinData.Height = height;

				WinData.m_ProcessEvent(e); // call event call back func in Window
			}
		);

		glfwSetWindowCloseCallback(m_NativeWindow,
			[](GLFWwindow* window)
			{
				WindowData& WinData = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent e;

				WinData.m_ProcessEvent(e); // call event call back func in Window
			}
		);

		glfwSetKeyCallback(m_NativeWindow,
			[](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& WinData = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					WinData.m_ProcessEvent(e); // call event call back func in Window
				}
				break;
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					WinData.m_ProcessEvent(e); // call event call back func in Window
				}
				break;
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, 1); // TODO:: Get Repeat Count
					WinData.m_ProcessEvent(e); // call event call back func in Window
				}
				break;
				default:
					break;
				}
			}
		);

		glfwSetCharCallback(m_NativeWindow,
			[](GLFWwindow* window, unsigned int c)
			{
				WindowData& WinData = *(WindowData*)glfwGetWindowUserPointer(window);
				KeyTypedEvent e(c);

				WinData.m_ProcessEvent(e); // call event call back func in Window
			});

		glfwSetCursorPosCallback(m_NativeWindow,
			[](GLFWwindow* window, double xpos, double ypos)
			{
				WindowData& WinData = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent e((float)xpos, (float)ypos);

				WinData.m_ProcessEvent(e); // call event call back func in Window
			}
		);

		glfwSetMouseButtonCallback(m_NativeWindow,
			[](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& WinData = *(WindowData*)glfwGetWindowUserPointer(window);
				
				switch (action)
				{
				case GLFW_PRESS:
				{
					MousePressedEvent e(button);
					WinData.m_ProcessEvent(e); // call event call back func in Window
				}
				break;
				case GLFW_RELEASE:
				{
					MouseReleasedEvent e(button);
					WinData.m_ProcessEvent(e); // call event call back func in Window
				}
				break;
				default:
					break;
				}
			}
		);

		glfwSetScrollCallback(m_NativeWindow,
			[](GLFWwindow* window, double xoffset, double yoffset)
			{
				WindowData& WinData = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrollEvent e((float)xoffset, (float)yoffset);

				WinData.m_ProcessEvent(e); // call event call back func in Window
			}
		);
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_NativeWindow);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVsync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.IsVsync = enabled;
	}
}