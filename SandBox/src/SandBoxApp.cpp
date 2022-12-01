#include <SnapEngine.h>

class ExampleLayer : public SnapEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(400.0f, -400.0f, 300.0f, -300.0f, 0.0f, 1.0f)
	{
		m_Texture = SnapEngine::SnapPtr<SnapEngine::Texture2D>(SnapEngine::Texture2D::Creat("assets/SpriteSheet.png"));
		m_SubTexture = SnapEngine::SubTexture2D::CreatFromCoords(m_Texture, { 1.0f, 2.0f }, { 400.0f, 600.0f });
		m_FrameBuffer = SnapEngine::SnapPtr<SnapEngine::FrameBuffer>(SnapEngine::FrameBuffer::Creat(Width, Height));
	}

	~ExampleLayer() {}

	void Update(SnapEngine::TimeStep Time) override
	{
		// Update Frame
		
		m_Position += m_Velocity * Time.GetSeconds();
		
		if (m_Position.x + m_Scale.x * 0.5f >= HalfWidth) { m_Position.x = HalfWidth - m_Scale.x * 0.5f; m_Velocity.x *= -1.0f; }
		if (m_Position.x - m_Scale.x * 0.5f <= -HalfWidth) { m_Position.x = -HalfWidth + m_Scale.x * 0.5f; m_Velocity.x *= -1.0f; }
		if (m_Position.y + m_Scale.y * 0.5f >= HalfHeight) { m_Position.y = HalfHeight - m_Scale.y * 0.5f; m_Velocity.y *= -1.0f; }
		if (m_Position.y - m_Scale.y * 0.5f <= -HalfHeight) { m_Position.y = -HalfHeight + m_Scale.y * 0.5f; m_Velocity.y *= -1.0f; }
		
		// Render

		SnapEngine::Renderer2D::ResetStats();
		
		m_FrameBuffer->Bind(); // Record Scene into frame buffer
		
		SnapEngine::RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		SnapEngine::RendererCommand::Clear();

		SnapEngine::Renderer2D::Begin(m_Camera);
		{
			SnapEngine::Renderer2D::DrawQuad(m_SubTexture,
				m_Position, m_Scale);
		}
		SnapEngine::Renderer2D::End();

		m_FrameBuffer->UnBind(); // Stop Recording
	}

	void ImGuiRender() override
	{
		static bool DockSpace_Open = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &DockSpace_Open, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Exit")) { SnapEngine::Application::Get().Close(); }
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		DrawImGuiDockableWindows();

		ImGui::End();
	}


	void DrawImGuiDockableWindows()
	{
		ImGui::Begin("Scene View");
		ImGui::Image((ImTextureID)m_FrameBuffer->GetTextureID(), ImVec2(256.0f, 256.0f));
		ImGui::End();
	}

	void ProcessEvent(SnapEngine::IEvent& e) override
	{
		SnapEngine::EventDispatcher dispatcher(e);
		dispatcher.DispatchEvent<SnapEngine::WindowResizeEvent>(SNAP_BIND_FUNCTION(ExampleLayer::WindowResizeCallBack));
		//dispatcher.DispatchEvent<SnapEngine::MouseScrollEvent>(SNAP_BIND_FUNCTION(ExampleLayer::MouseScrollCallBack));
	}

private:
	SnapEngine::OrthoGraphicsCamera m_Camera;
	SnapEngine::SnapPtr<SnapEngine::Texture2D> m_Texture;
	SnapEngine::SnapPtr<SnapEngine::SubTexture2D> m_SubTexture;
	SnapEngine::SnapPtr<SnapEngine::FrameBuffer> m_FrameBuffer;

	// Rendering
	glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 m_Scale = { 100.0f, 100.0f, 1.0f };
	glm::vec3 m_Velocity = { 100.0f, 50.0f, 0.0f };

	// Event variables
	float Width = 800.0f;
	float Height = 600.0f;
	float HalfWidth = 400.0f;
	float HalfHeight = 300.0f;

	float m_Zoom = 1.0f;

private:
	// Event Dispatch CallBacks
	bool WindowResizeCallBack(SnapEngine::WindowResizeEvent& e)
	{
		Width = (float)e.GetWidth();
		Height = (float)e.GetHeight();
		HalfWidth = Width * 0.5f;
		HalfHeight = Height * 0.5f;

		// Change Camera Projection According To New Width and New Height
		m_Camera = SnapEngine::OrthoGraphicsCamera(HalfWidth, -HalfWidth, HalfHeight, -HalfHeight, 0.0f, 1.0f);

		return false;
	}

	bool MouseScrollCallBack(SnapEngine::MouseScrollEvent& e)
	{
		m_Zoom -= e.GetOffsetY() * 0.01f;
		Width *= m_Zoom;
		Height *= m_Zoom;

		SNAP_DEBUG("Zoom = {}", m_Zoom);

		// Change Camera Projection According To New Width and New Height
		m_Camera = SnapEngine::OrthoGraphicsCamera(HalfWidth, -HalfWidth, HalfHeight, -HalfHeight, 0.0f, 1.0f);
		
		return false;
	}
};

class SandBox : public SnapEngine::Application
{
public:
	SandBox()
	{
		SNAP_INFO("Welcome To First App!");
		PushLayer(new ExampleLayer());
	}

	~SandBox() {}
private:

};

SnapEngine::Application* SnapEngine::CreatApplication()
{
	return new SandBox();
}