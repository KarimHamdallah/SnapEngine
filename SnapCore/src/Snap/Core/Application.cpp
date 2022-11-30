#include "SnapPCH.h"
#include "Application.h"
#include <GLFW/glfw3.h>

#include <Snap/Renderer/Renderer.h>
#include <Snap/Renderer/Renderer2D.h>

namespace SnapEngine
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		s_Instance = this;
		m_Window = std::unique_ptr<IWindow>(IWindow::Creat());
		m_Window->SetProcessEventFuncPtr(SNAP_BIND_FUNCTION(Application::ProcessEvent));
		m_Window->SetVsync(true);

		// Initialize ImGui
		m_ImGuiLayer = new ImGuiLayer();
		PushOverLayer(m_ImGuiLayer);

		// Initialize Renderer
		Renderer::Init();
		Renderer2D::Init();
	}

	Application::~Application()
	{
		Renderer2D::ShutDown();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime(); // Platform::GetTime()
			TimeStep m_TimeStep = time - m_LastTime;
			m_LastTime = time;

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayersStack)
					layer->Update(m_TimeStep);
			}

			// Begin Render ImGui
			m_ImGuiLayer->Begin();

			for (Layer* layer : m_LayersStack)
				layer->ImGuiRender();

			// End Render Imgui
			m_ImGuiLayer->End();


			m_Window->OnUpdate();
		}
	}
	
	void Application::ProcessEvent(IEvent& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.DispatchEvent<WindowCloseEvent>(SNAP_BIND_FUNCTION(Application::OnWindowClose));
		dispatcher.DispatchEvent<WindowResizeEvent>(SNAP_BIND_FUNCTION(Application::OnWindowResize));

		for (auto it = m_LayersStack.end(); it != m_LayersStack.begin();)
		{
			(*--it)->ProcessEvent(e);
			if (e.m_Handeled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayersStack.PushLayer(layer);
		layer->Start();
	}

	void Application::PushOverLayer(Layer* overlayer)
	{
		m_LayersStack.PushOverLayer(overlayer);
		overlayer->Start();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::SetViewPort(0, 0, e.GetWidth(), e.GetHeight());

		return false;
	}
}