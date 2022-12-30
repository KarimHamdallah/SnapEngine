#include "SnapPCH.h"
#include "SnapEditor.h"

namespace SnapEngine
{
	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialoge::OpenFile("Snap Scene (*.snap)\0*.snap\0");
		if (!filepath.empty())
		{
			OpenScene(filepath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& filepath)
	{
		if (m_CurrentSceneState != SceneState::EDIT)
			StopScene();

		if (!filepath.empty())
		{

			m_Scene = CreatSnapPtr<Scene>(); // Creat Empty New Scene
			
			SceneSerializer serializer(m_Scene); // Load Into New Scene
			serializer.DeSerializeScene(filepath.string());

			m_Scene->ResizeViewPort((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
			m_SceneHierarchyPanel.SetScene(m_Scene);

			// TempScene Is Another Reference To the same object
			m_TempScene = m_Scene;
		}
	}

	void EditorLayer::NewScene()
	{
		m_Scene = CreatSnapPtr<Scene>();
		m_Scene->ResizeViewPort((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		m_SceneHierarchyPanel.SetScene(m_Scene);
	}

	void EditorLayer::SaveScene()
	{
		std::string filepath = FileDialoge::SaveFile("Snap Scene (*.snap)\0*.snap\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_Scene);
			serializer.SerializeScene(filepath);
		}
	}




	/////////////////////////// Event CallBacks ///////////////////////////////////////////

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
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
				SaveScene();
		}
		break;

		case Key::O:
		{
			bool cntrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
			
			if (cntrl)
				OpenScene();
		}
		break;

		case Key::N:
		{
			bool cntrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
			if (cntrl)
				NewScene();
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
		if (m_CurrentSceneState == SceneState::EDIT)
		{
			if (e.GetMouseButton() == (int)MouseButton::MouseButtonLeft && m_ViewPortFocused)
			{
				if (m_ViewPortHavored && !ImGuizmo::IsOver() && !m_EditorCamera.IsActive())
					m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			}
		}
		return false;
	}

	void EditorLayer::ProcessEvent(IEvent& e)
	{
		switch (m_CurrentSceneState)
		{
		case SnapEngine::EditorLayer::SceneState::PLAY:
		{
			m_Scene->ProcessEvents(&e); // Process Scene Scripts Events
		}
			break;
		case SnapEngine::EditorLayer::SceneState::EDIT:
		{
			if (m_ViewPortFocused && m_ViewPortHavored)
				m_EditorCamera.ProcessEvents(e);
		}
			break;
		default:
			break;
		}

		if (m_ContentBrowserPanel.IsWindowFocused() && m_ContentBrowserPanel.IsWindowHovered())
			m_ContentBrowserPanel.ProcessEvents(e);

		EventDispatcher dispatcher(e);
		dispatcher.DispatchEvent<KeyPressedEvent>(SNAP_BIND_FUNCTION(EditorLayer::OnKeyPressed));
		dispatcher.DispatchEvent<MousePressedEvent>(SNAP_BIND_FUNCTION(EditorLayer::OnMousePressed));
	}







	void EditorLayer::StartDockSpace()
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
		//style.WindowMinSize.x = 450.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Exit")) { Application::Get().Close(); }
				
				if (ImGui::MenuItem("New...", "Cntrl+N"))
					NewScene();

				if (ImGui::MenuItem("Open...", "Cntrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save As...", "Cntrl+Shift+S"))
					SaveScene();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}

	void EditorLayer::EndDockSpace()
	{
		ImGui::End();
	}

	void EditorLayer::UIToolbar()
	{

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 2.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });

		ImGui::Begin("##Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		

		SnapPtr<Texture2D> Icon;

		switch (m_CurrentSceneState)
		{
		case SnapEngine::EditorLayer::SceneState::PLAY: Icon = m_StopIcon;
			break;
		case SnapEngine::EditorLayer::SceneState::EDIT: Icon = m_PlayIcon;
			break;
		default:
			break;
		}
		
		float size = ImGui::GetWindowHeight() - 4.0f;

		ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)Icon->getID(), ImVec2{ size, size }))
		{
			if (m_CurrentSceneState == SceneState::EDIT)
				PlayScene();
			else if(m_CurrentSceneState == SceneState::PLAY)
				StopScene();
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::End();
	}

	void EditorLayer::PlayScene()
	{
		m_CurrentSceneState = SceneState::PLAY;

		if(!m_KeepRunTimeChanges)
			m_Scene = Scene::Copy(m_TempScene);
		m_Scene->OnRunTimeStart();
	}

	void EditorLayer::StopScene()
	{
		m_CurrentSceneState = SceneState::EDIT;
		m_Scene->OnRunTimeStop();

		if (!m_KeepRunTimeChanges)
			m_Scene = m_TempScene;
	}


























	Application* CreatApplication()
	{
		return new SnapEditor();
	}
}