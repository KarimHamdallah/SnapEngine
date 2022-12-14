#pragma once
#include <Snap/Core/Window.h>
#include <Snap/Events/WindowEvent.h>
#include <Snap/Core/LayerStack.h>
#include <Snap/ImGui/ImGuiLayer.h>

namespace SnapEngine
{
	class Application
	{
	public:
		Application(const std::string& name = "SnapEngine Application", uint32_t Width = 800, uint32_t Height = 600);
		virtual ~Application();

		void Run();
		void ProcessEvent(IEvent& e);

		void PushLayer(Layer* layer);
		void PushOverLayer(Layer* overlayer);

		inline static Application& Get() { return *s_Instance; }
		inline IWindow& GetWindow() { return *m_Window; }

		inline void Close() { m_Running = false; }
		inline ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
	private:
		std::unique_ptr<IWindow> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayersStack;
		
		float m_LastTime = 0.0f;

		bool m_Minimized = false;

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
		static Application* s_Instance;
	};

	// Should Be Defined In Client Project
	Application* CreatApplication();
}