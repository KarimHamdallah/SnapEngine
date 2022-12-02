#include <SnapEngine.h>


namespace SnapEngine
{

	class EditorLayer : public SnapEngine::Layer
	{
	public:
		EditorLayer()
			: Layer("Example"), m_Camera(400.0f, -400.0f, 300.0f, -300.0f, 0.0f, 1.0f)
		{
			m_Texture = SnapPtr< Texture2D>(Texture2D::Creat("assets/SpriteSheet.png"));
			m_SubTexture = SubTexture2D::CreatFromCoords(m_Texture, { 1.0f, 2.0f }, { 400.0f, 600.0f });
			m_FrameBuffer = SnapPtr< FrameBuffer>(FrameBuffer::Creat(800.0f, 600.0f));
		}

		~EditorLayer() {}

		void Update( TimeStep Time) override
		{

			dt = Time;
			// Update Frame
			/*
			m_Position += m_Velocity * Time.GetSeconds();

			float HalfWidth = m_ViewPortSize.x * 0.5f;
			float HalfHeight = m_ViewPortSize.y * 0.5f;

			if (m_Position.x + m_Scale.x * 0.5f >= HalfWidth) { m_Position.x = HalfWidth - m_Scale.x * 0.5f; m_Velocity.x *= -1.0f; }
			if (m_Position.x - m_Scale.x * 0.5f <= -HalfWidth) { m_Position.x = -HalfWidth + m_Scale.x * 0.5f; m_Velocity.x *= -1.0f; }
			if (m_Position.y + m_Scale.y * 0.5f >= HalfHeight) { m_Position.y = HalfHeight - m_Scale.y * 0.5f; m_Velocity.y *= -1.0f; }
			if (m_Position.y - m_Scale.y * 0.5f <= -HalfHeight) { m_Position.y = -HalfHeight + m_Scale.y * 0.5f; m_Velocity.y *= -1.0f; }
			*/

			if (m_ViewPortFocused)
			{
				if (Input::IsKeyPressed(Key::W))
					m_Position.y += m_Velocity.y * dt;
				if (Input::IsKeyPressed(Key::S))
					m_Position.y -= m_Velocity.y * dt;
				if (Input::IsKeyPressed(Key::D))
					m_Position.x += m_Velocity.x * dt;
				if (Input::IsKeyPressed(Key::A))
					m_Position.x -= m_Velocity.x * dt;
			}
			// Render

			 Renderer2D::ResetStats();

			m_FrameBuffer->Bind(); // Record Scene into frame buffer

			 RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			 RendererCommand::Clear();

			 Renderer2D::Begin(m_Camera);
			{
				 Renderer2D::DrawQuad(m_SubTexture,
					m_Position, m_Scale);
			}
			 Renderer2D::End();

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
					if (ImGui::MenuItem("Exit")) {  Application::Get().Close(); }
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::Begin("Hiererachy");
			ImGui::End();

			// ViewPort Window
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("ViewPort");

			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHavored = ImGui::IsWindowHovered();

			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused || !m_ViewPortHavored);
			
			{ // Resize Renderer Area
				ImVec2 viewportsize = ImGui::GetContentRegionAvail(); // Get This ImGui Window Size
				if (m_ViewPortSize != *((glm::vec2*)&viewportsize) && viewportsize.x != 0 && viewportsize.y != 0)
				{
					m_ViewPortSize = { viewportsize.x, viewportsize.y }; // Record new ViewPort Window Size
					m_FrameBuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y); // Reset Frame Buffer To new ViewPort Window Size

					// Change Camera Projection According To New Width and New Height of ViewPortWindow
					m_Camera = SnapEngine::OrthoGraphicsCamera(
						m_ViewPortSize.x * 0.5f, m_ViewPortSize.x * -0.5f,
						m_ViewPortSize.y * 0.5f, m_ViewPortSize.y * -0.5f,
						0.0f, 1.0f);
				}
			}

			ImGui::Image((ImTextureID)m_FrameBuffer->GetTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();
			ImGui::PopStyleVar();

			ImGui::End();
		}

		void ProcessEvent( IEvent& e) override
		{
		}

	private:
		OrthoGraphicsCamera m_Camera;
		SnapPtr< Texture2D> m_Texture;
		SnapPtr< SubTexture2D> m_SubTexture;
		SnapPtr< FrameBuffer> m_FrameBuffer;

		glm::vec2 m_ViewPortSize; // ImGui ViewPort Window Size
		bool m_ViewPortFocused = false;
		bool m_ViewPortHavored = false;

		float dt = 0.0f;

		// Rendering
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_Scale = { 100.0f, 100.0f, 1.0f };
		glm::vec3 m_Velocity = { 100.0f, 100.0f, 0.0f };

	private:
	};

	class SnapEditor : public SnapEngine::Application
	{
	public:
		SnapEditor()
			: Application("SnapEditor")
		{
			SNAP_INFO("Welcome To First App!");
			PushLayer(new EditorLayer());
		}

		~SnapEditor() {}
	private:

	};

	Application* CreatApplication()
	{
		return new SnapEditor();
	}
}