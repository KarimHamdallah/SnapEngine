#include <SnapEngine.h>


namespace SnapEngine
{

	class EditorLayer : public SnapEngine::Layer
	{
	public:
		EditorLayer()
			: Layer("Example")
		{
			m_FrameBuffer = SnapPtr< FrameBuffer>(FrameBuffer::Creat(800.0f, 600.0f));


			/// Scene ///////////////////////////////////////////////

			m_Scene = CreatSnapPtr<Scene>();

			m_Camera = m_Scene->CreatEntity("Camera");
			m_Camera.AddComponent<CameraComponent>(10.0f, -1.0f, 1.0f).m_IsMain = true;

			m_Sprite = m_Scene->CreatEntity("Sprite", { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, 0.0f);
			m_Sprite.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}

		~EditorLayer() {}

		void Update( TimeStep Time) override
		{

			{ // Resize Renderer Area
				if (m_ViewPortSize.x > 0 && m_ViewPortSize.y > 0
					&& (m_FrameBuffer->GetWidth() != m_ViewPortSize.x || m_FrameBuffer->GetHeight() != m_ViewPortSize.y))
				{
					m_FrameBuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y); // Reset Frame Buffer To new ViewPort Window Size

					// Change Scene Cameras Projection According To New Width and New Height of ViewPortWindow
					m_Scene->ResizeViewPort(m_ViewPortSize.x, m_ViewPortSize.y);
				}
			}


			if (m_ViewPortFocused)
			{
				m_Scene->Update(Time);
			}

			// Render
			m_FrameBuffer->Bind(); // Record Scene into frame buffer

			m_Scene->Render();

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

			ImGui::Begin("Settings");
			auto& sprite_color = m_Sprite.GetComponent<SpriteRendererComponent>().m_Color;
			ImGui::ColorEdit4("Sprite Color", glm::value_ptr(sprite_color));

			auto& cam = m_Camera.GetComponent<CameraComponent>().m_Camera;
			float orthosize = cam.GetOrthoGraphicSize();
			if (ImGui::DragFloat("OrthoGraphic Size", &orthosize, 0.01f))
				cam.SetOrthoGraphicSize(orthosize);
			
			
			ImGui::End();

			// ViewPort Window
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("ViewPort");

			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHavored = ImGui::IsWindowHovered();

			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused || !m_ViewPortHavored);
			
			ImVec2 viewportsize = ImGui::GetContentRegionAvail(); // Get This ImGui Window Size
			m_ViewPortSize = { viewportsize.x, viewportsize.y };

			ImGui::Image((ImTextureID)m_FrameBuffer->GetTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();
			ImGui::PopStyleVar();

			ImGui::End();
		}

	private:
		SnapPtr< FrameBuffer> m_FrameBuffer;
		SnapPtr<Scene> m_Scene;
		
		///////// Entites ////////////////
		Entity m_Camera;
		Entity m_Sprite;
		/////////////////////////////////

		/////////////////// ViewPort ////////////////////////////
		glm::vec2 m_ViewPortSize = { 800.0f, 600.0f }; // ImGui ViewPort Window Size
		bool m_ViewPortFocused = false;
		bool m_ViewPortHavored = false;
		/////////////////////////////////////////////////////////

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