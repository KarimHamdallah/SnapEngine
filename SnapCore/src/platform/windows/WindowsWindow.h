#pragma once
#include <Snap/Core/Window.h>
#include <Snap/Renderer/GraphicsContext.h>
#include <GLFW/glfw3.h>

namespace SnapEngine
{
	class WindowsWindow : public IWindow
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline virtual void* GetNativeWindow() const override { return m_NativeWindow; }

		inline void SetProcessEventFuncPtr(const ProcessEventFuncPtr& callback) override { m_Data.m_ProcessEvent = callback; }
		void SetVsync(bool enabled) override;
		bool IsVsync() const override { return m_Data.IsVsync; }

	private:
		virtual void init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_NativeWindow;
		struct WindowData
		{
			std::string Title;
			unsigned int Width;
			unsigned int Height;
			bool IsVsync = false;
			ProcessEventFuncPtr m_ProcessEvent; // funcptr to func take event ref and return void
		};

		WindowData m_Data;
		std::unique_ptr<GraphicsContext> m_Context;
	};
}