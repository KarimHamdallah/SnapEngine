#include <SnapEngine.h>
#include <Snap/Scene/Scripts/CameraControllerScript.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include <platform/Utils/PlatformUtils.h>


namespace SnapEngine
{

	class EditorLayer : public SnapEngine::Layer
	{
	public:
		EditorLayer()
			: Layer("Example")
		{
			FrameBufferSpecifications FrameBufferSpecs;
			FrameBufferSpecs.Attachments = 
			{
				FrameBufferTextureFormat::RGBA8, // Color Attachment For Rendering
				FrameBufferTextureFormat::RED_INTEGER, // Color Attachment For MousePicking
				FrameBufferTextureFormat::DEPTH24STECNCIL8
			};
			FrameBufferSpecs.Width = 800.0f;
			FrameBufferSpecs.Height = 600.0f;
			m_FrameBuffer = SnapPtr< FrameBuffer>(FrameBuffer::Creat(FrameBufferSpecs));
			m_RealTimeFrameBuffer = SnapPtr< FrameBuffer>(FrameBuffer::Creat(FrameBufferSpecs));


			/// Scene ///////////////////////////////////////////////

			m_Scene = CreatSnapPtr<Scene>();
			m_SceneHierarchyPanel.SetScene(m_Scene);
#if 0
			m_Camera = m_Scene->CreatEntity("Camera");
			m_Camera.AddComponent<CameraComponent>(10.0f, -100.0f, 100.0f).m_IsMain = true;
			m_Camera.AddComponent<CppScriptComponent>().Bind<CameraControllerScript>();

			m_Sprite = m_Scene->CreatEntity("Sprite", { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f });
			m_Sprite.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			m_Sprite2 = m_Scene->CreatEntity("WhiteSprite", { 1.5f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f });
			m_Sprite2.AddComponent<SpriteRendererComponent>(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
#endif
		}

		~EditorLayer() {}

		void Update(TimeStep Time) override
		{

			{ // Resize Renderer Area
				if (m_ViewPortSize.x > 0 && m_ViewPortSize.y > 0
					&& (m_FrameBuffer->GetWidth() != m_ViewPortSize.x || m_FrameBuffer->GetHeight() != m_ViewPortSize.y))
				{
					m_FrameBuffer->Resize((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y); // Reset Frame Buffer To new ViewPort Window Size

					// Change Scene Cameras Projection According To New Width and New Height of ViewPortWindow
					m_EditorCamera.SetViewPortSize(m_ViewPortSize.x, m_ViewPortSize.y);
				}

				if (m_MainCameraViewSize.x > 0 && m_MainCameraViewSize.y > 0
					&& (m_RealTimeFrameBuffer->GetWidth() != m_MainCameraViewSize.x || m_FrameBuffer->GetHeight() != m_MainCameraViewSize.y))
				{
					m_RealTimeFrameBuffer->Resize((uint32_t)m_MainCameraViewSize.x, (uint32_t)m_MainCameraViewSize.y); // Reset Frame Buffer To new ViewPort Window Size

					// Change Scene Cameras Projection According To New Width and New Height of ViewPortWindow
					m_Scene->ResizeViewPort(m_MainCameraViewSize.x, m_MainCameraViewSize.y);
				}
			}


			// Update Scripts
			if (m_MainCameraViewFocused && m_MainCameraViewHavored)
			{
				m_Scene->UpdateRunTime(Time);
			}
			else
				m_EditorCamera.UpdateCamera(Time);
			

			// Render
			m_FrameBuffer->Bind(); // Record Scene into frame buffer

			Renderer2D::ResetStats();
			RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RendererCommand::Clear();
			m_FrameBuffer->ClearAttachment(1, -1);

			m_Scene->UpdateEditor(Time, m_EditorCamera);


			auto [mx, my] = ImGui::GetMousePos();
			mx -= m_ViewPortBounds[0].x;
			my -= m_ViewPortBounds[0].y;

			my = m_ViewPortSize.y - my;

			int MouseX = (int)mx;
			int MouseY = (int)my;

			if (MouseX >= 0 && MouseY >= 0 && MouseX <= (int)m_ViewPortSize.x && MouseY <= (int)m_ViewPortSize.y)
			{
				int pixel = m_FrameBuffer->ReadPixel(1, MouseX, MouseY);
				m_HoveredEntity = pixel == -1 ? Entity() : Entity((Entity::Handel)pixel, m_Scene.get());
				//SNAP_DEBUG("pixel = {}", pixel);
			}

			m_FrameBuffer->UnBind(); // Stop Recording



			m_RealTimeFrameBuffer->Bind(); // Record Scene into frame buffer

			Renderer2D::ResetStats();
			RendererCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
			RendererCommand::Clear();

			m_Scene->Render();

			m_RealTimeFrameBuffer->UnBind(); // Stop Recording
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
			ImGuiStyle& style = ImGui::GetStyle();
			float MinWindowSizeX = style.WindowMinSize.x;
			//style.WindowMinSize.x = 450.0f;
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			
			//style.WindowMinSize.x = MinWindowSizeX;

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Options"))
				{
					if (ImGui::MenuItem("Exit")) {  Application::Get().Close(); }
					if (ImGui::MenuItem("New...", "Cntrl+N"))
					{
						m_Scene = CreatSnapPtr<Scene>();
						m_Scene->ResizeViewPort((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
						m_SceneHierarchyPanel.SetScene(m_Scene);
					}
					if (ImGui::MenuItem("Open...", "Cntrl+O"))
					{
						std::string filepath = FileDialoge::OpenFile("Snap Scene (*.snap)\0*.snap\0");
						if (!filepath.empty())
						{
							m_Scene = CreatSnapPtr<Scene>();
							m_Scene->ResizeViewPort((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
							m_SceneHierarchyPanel.SetScene(m_Scene);

							SceneSerializer serializer(m_Scene);
							serializer.DeSerializeScene(filepath);
						}
					}
					if (ImGui::MenuItem("Save As...", "Cntrl+Shift+S"))
					{
						std::string filepath = FileDialoge::SaveFile("Snap Scene (*.snap)\0*.snap\0");
						if (!filepath.empty())
						{
							SceneSerializer serializer(m_Scene);
							serializer.SerializeScene(filepath);
						}
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			m_SceneHierarchyPanel.ImGuiRender();
			m_ContentBrowserPanel.ImGuiRender();

			ImGui::Begin("Settings");
			static bool check = true;
			if (ImGui::Checkbox("Enable Camera", &check))
				m_Camera.GetComponent<CameraComponent>().m_IsMain = check;

			if (ImGui::DragFloat("Snapping", &m_Snapping, 0.01f))
				if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
					m_Snapping = 45.0f;
				else
					m_Snapping = 0.5f;

			ImGui::End();

			// ViewPort Window
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("ViewPort");

			auto ViewPortOffset = ImGui::GetCursorPos();

			m_ViewPortFocused = ImGui::IsWindowFocused();
			m_ViewPortHavored = ImGui::IsWindowHovered();

			Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewPortFocused || !m_ViewPortHavored);
			
			ImVec2 viewportsize = ImGui::GetContentRegionAvail(); // Get This ImGui Window Size
			m_ViewPortSize = { viewportsize.x, viewportsize.y };

			ImGui::Image((ImTextureID)m_FrameBuffer->GetColorAttachmentTextureID(), ImVec2{ m_ViewPortSize.x, m_ViewPortSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			
			// Calculate ViewPort Window Size 
			auto WindowSize = ImGui::GetWindowSize();
			ImVec2 minBound = ImGui::GetWindowPos();

			minBound.x += ViewPortOffset.x;
			minBound.y += ViewPortOffset.y;

			ImVec2 maxBound = { minBound.x + WindowSize.x, minBound.y + WindowSize.y };
			m_ViewPortBounds[0] = { minBound.x, minBound.y };
			m_ViewPortBounds[1] = { maxBound.x, maxBound.y };

			// Draw ImGuizmo inside ViewPort Widnow
			Entity SelectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();

			if (SelectedEntity)
			{
				Entity MainCameraEntity = m_Scene->GetMainCameraEntity();
				if (MainCameraEntity)
				{
					auto& transform = MainCameraEntity.GetComponent<TransformComponent>();
					auto& cam = MainCameraEntity.GetComponent<CameraComponent>();
					
					bool IsOrthoGraphic = cam.m_Camera.GetProjectionType() ==
						SceneCamera::ProjectionType::OrthoGraphic ? true : false;

					ImGuizmo::SetOrthographic(false);
					ImGuizmo::SetDrawlist();
					float WindowPosX = ImGui::GetWindowPos().x;
					float WindowPosY = ImGui::GetWindowPos().y;
					float WindowWidth = ImGui::GetWindowWidth();
					float WindowHeight = ImGui::GetWindowHeight();
					ImGuizmo::SetRect(WindowPosX, WindowPosY, WindowWidth, WindowHeight);

					
					auto& e_transform = SelectedEntity.GetComponent<TransformComponent>();

					glm::mat4 EntityTransformMatrix = SelectedEntity.GetComponent<TransformComponent>().GetTransformMatrix();
					glm::mat4 CameraTransformMatrix = transform.GetTransformMatrix();
					glm::mat4 ViewMatrix = /*glm::inverse(CameraTransformMatrix);*/ m_EditorCamera.GetViewMatrix();
					glm::mat4 ProjectionMatrix = /*cam.m_Camera.GetProjectionMatrix();*/ m_EditorCamera.GetProjectionMatrix();

					ImGuizmo::Manipulate(glm::value_ptr(ViewMatrix), glm::value_ptr(ProjectionMatrix),
						(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(EntityTransformMatrix),
						nullptr, Input::IsKeyPressed(Key::LeftControl) ? &m_Snapping : nullptr);
						
					if (ImGuizmo::IsUsing())
					{
						ImGuizmo::DecomposeMatrixToComponents(
							glm::value_ptr(EntityTransformMatrix),
							glm::value_ptr(e_transform.m_Position),
							glm::value_ptr(e_transform.m_Rotation),
							glm::value_ptr(e_transform.m_Scale));
					}
				}
			}
			
			ImGui::End();
			ImGui::PopStyleVar();

			ImGui::Begin("MainCameraView");

			m_MainCameraViewFocused = ImGui::IsWindowFocused();
			m_MainCameraViewHavored = ImGui::IsWindowHovered();
			
			if (!m_ViewPortFocused || !m_ViewPortHavored)
				Application::Get().GetImGuiLayer()->BlockEvents(!m_MainCameraViewFocused || !m_MainCameraViewHavored);

			m_MainCameraViewSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

			ImGui::Image((ImTextureID)m_RealTimeFrameBuffer->GetColorAttachmentTextureID(), ImVec2{ m_MainCameraViewSize.x, m_MainCameraViewSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			
			ImGui::End();


			if (m_ContentBrowserPanel.IsWindowFocused() && m_ContentBrowserPanel.IsWindowHovered())
				Application::Get().GetImGuiLayer()->BlockEvents(false);

			ImGui::End();
		}

	private:
		SnapPtr< FrameBuffer> m_FrameBuffer;
		SnapPtr< FrameBuffer> m_RealTimeFrameBuffer;
		SnapPtr<Scene> m_Scene;
		
		///////// Entites ////////////////
		Entity m_Camera;
		Entity m_Sprite;
		Entity m_Sprite2;
		/////////////////////////////////

		/////////////////// ViewPort ////////////////////////////
		glm::vec2 m_ViewPortSize = { 800.0f, 600.0f }; // ImGui ViewPort Window Size
		bool m_ViewPortFocused = false;
		bool m_ViewPortHavored = false;

		glm::vec2 m_ViewPortBounds[2];

		glm::vec2 m_MainCameraViewSize = { 800.0f, 600.0f }; // ImGui ViewPort Window Size
		bool m_MainCameraViewFocused = false;
		bool m_MainCameraViewHavored = false;
		/////////////////////////////////////////////////////////

		//////////////// Editor ///////////////
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		int m_GizmoType = 0;
		float m_Snapping = 0.5f;
		EditorCamera m_EditorCamera;
		Entity m_HoveredEntity;
	private:

		virtual void ProcessEvent(IEvent& e) override
		{
			if (m_MainCameraViewFocused && m_MainCameraViewHavored)
				m_Scene->ProcessEvents(&e); // Process Scene Scripts Events
			
			if(m_ViewPortFocused && m_ViewPortHavored)
				m_EditorCamera.ProcessEvents(e);
			
			if(m_ContentBrowserPanel.IsWindowFocused() && m_ContentBrowserPanel.IsWindowHovered())
				m_ContentBrowserPanel.ProcessEvents(e);

			EventDispatcher dispatcher(e);
			dispatcher.DispatchEvent<KeyPressedEvent>(SNAP_BIND_FUNCTION(EditorLayer::OnKeyPressed));
			dispatcher.DispatchEvent<MousePressedEvent>(SNAP_BIND_FUNCTION(EditorLayer::OnMousePressed));
		}

		bool OnKeyPressed(KeyPressedEvent& e)
		{
			if (e.GetRepeatCount() > 0 || m_EditorCamera.IsActive() || !m_ViewPortFocused)
				return false;

			switch ((Key)e.GetKeyCode())
			{
			case Key::S:
			{
				bool cntrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
				bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

				if (cntrl && shift)
				{
					// Save Scene
					std::string filepath = FileDialoge::SaveFile("Snap Scene (*.snap)\0*.snap\0");
					if (!filepath.empty())
					{
						SceneSerializer serializer(m_Scene);
						serializer.SerializeScene(filepath);
					}
				}
			}
			break;
			case Key::O:
			{
				bool cntrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
				if (cntrl)
				{
					// Open Scene
					std::string filepath = FileDialoge::OpenFile("Snap Scene (*.snap)\0*.snap\0");
					if (!filepath.empty())
					{
						m_Scene = CreatSnapPtr<Scene>();
						m_Scene->ResizeViewPort((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
						m_SceneHierarchyPanel.SetScene(m_Scene);

						SceneSerializer serializer(m_Scene);
						serializer.DeSerializeScene(filepath);
					}
				}
			}
			break;
			case Key::N:
			{
				bool cntrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
				if (cntrl)
				{
					// New Scene
					m_Scene = CreatSnapPtr<Scene>();
					m_Scene->ResizeViewPort((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
					m_SceneHierarchyPanel.SetScene(m_Scene);
				}
			}
			break;
			case Key::Q:
			{
				m_GizmoType = -1;
			}
			break;
			case Key::W:
			{
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}
			break;
			case Key::E:
			{
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
			}
			break;
			case Key::R:
			{
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			}
			break;
			default:
				break;
			};

			return true;
		}

		bool EditorLayer::OnMousePressed(MousePressedEvent& e)
		{
			if (e.GetMouseButton() == (int)MouseButton::MouseButtonLeft)
			{
				if (m_ViewPortHavored && !ImGuizmo::IsOver() && !m_EditorCamera.IsActive())
					m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}
			return false;
		}
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