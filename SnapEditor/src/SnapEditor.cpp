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
		switch (m_CurrentSceneState)
		{
		case SnapEngine::EditorLayer::SceneState::PLAY: StopScene(); break;
		case SnapEngine::EditorLayer::SceneState::Simulate: StopSimulationScene(); break;
		default: break;
		}

		if (!filepath.empty())
		{
			m_CurrentDeserializedScenePath = filepath;
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
		m_CurrentDeserializedScenePath = "";
		m_Scene = CreatSnapPtr<Scene>();
		m_Scene->ResizeViewPort((uint32_t)m_ViewPortSize.x, (uint32_t)m_ViewPortSize.y);
		m_SceneHierarchyPanel.SetScene(m_Scene);
	}

	void EditorLayer::SaveScene(const std::filesystem::path& path)
	{
		if (!path.empty())
		{
			m_CurrentDeserializedScenePath = path;
			SceneSerializer serializer(m_Scene);
			serializer.SerializeScene(path.string());
		}
	}

	void EditorLayer::SaveScene()
	{
		std::string filepath = FileDialoge::SaveFile("Snap Scene (*.snap)\0*.snap\0");
		if (!filepath.empty())
			SaveScene(filepath);
	}




	/////////////////////////// Event CallBacks ///////////////////////////////////////////

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0 || m_EditorCamera.IsActive() || !m_ViewPortFocused || m_CurrentSceneState != SceneState::EDIT)
			return false;

		switch ((Key)e.GetKeyCode())
		{
		
		case Key::S:
		{
			bool cntrl = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
			bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

			if (cntrl && shift)
				SaveScene(); // Dialoge
			else if (cntrl && !shift)
			{
				if (m_CurrentDeserializedScenePath.empty())
					SaveScene(); // Dialoge
				else
					SaveScene(m_CurrentDeserializedScenePath);
			}
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

		case Key::D:
		{
			if (Input::IsKeyPressed(Key::LeftControl))
				OnDuplicateEntity();
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

				if (ImGui::MenuItem("Save...", "Cntrl+S"))
				{
					if (m_CurrentDeserializedScenePath.empty())
						SaveScene(); // Dialoge
					else
						SaveScene(m_CurrentDeserializedScenePath);
				}

				if (ImGui::MenuItem("Save As...", "Cntrl+Shift+S"))
					SaveScene();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("ScriptingEngine"))
			{
				if (ImGui::MenuItem("Reload C# Assemblies"))
				{
					Scripting::ScriptingEngine::ReloadAssemby();
				}
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
		

		SnapPtr<Texture2D> PlayIcon = m_PlayIcon;
		SnapPtr<Texture2D> SimulateIcon = m_SimulateIcon;

		switch (m_CurrentSceneState)
		{
		case SnapEngine::EditorLayer::SceneState::PLAY: PlayIcon = m_StopIcon; break;
		case SnapEngine::EditorLayer::SceneState::Simulate: SimulateIcon = m_StopIcon; break;
		/*
		case SnapEngine::EditorLayer::SceneState::EDIT:
		{
			PlayIcon = m_PlayIcon;
			SimulateIcon = m_SimulateIcon;
		} break;
		*/
		default:
			break;
		}
		
		float size = ImGui::GetWindowHeight() - 4.0f;

		ImGui::SetCursorPosX((ImGui::GetContentRegionMax().x * 0.5f) - (size * 0.5f));
		if (ImGui::ImageButton((ImTextureID)PlayIcon->getID(), ImVec2{ size, size }))
		{
			if (m_CurrentSceneState == SceneState::EDIT)
				PlayScene();
			else if(m_CurrentSceneState == SceneState::PLAY)
				StopScene();
		}

		ImGui::SameLine();

		if (ImGui::ImageButton((ImTextureID)SimulateIcon->getID(), ImVec2{ size, size })) // Simulate
		{
			if (m_CurrentSceneState == SceneState::EDIT)
				PlaySimulateScene();
			else if (m_CurrentSceneState == SceneState::Simulate)
				StopSimulationScene();
		}

		// Pause And Step
		if (m_CurrentSceneState != SceneState::EDIT)
		{
			ImGui::SameLine();
			ImVec4 PressedColor(0.3f, 0.3f, 0.3f, 1.0f);
			ImVec4 ReleasedColor(1.0f, 1.0f, 1.0f, 0.0f);

			bool paused = m_Scene->IsPaused();

			ImGui::PushStyleColor(ImGuiCol_Button, paused ? PressedColor : ReleasedColor);
			if (ImGui::ImageButton((ImTextureID)m_PauseIcon->getID(), ImVec2{ size, size })) // Pause
			{
				m_Scene->SetPaused(!paused);
			}
			ImGui::PopStyleColor();

			ImGui::SameLine();

			if (ImGui::ImageButton((ImTextureID)m_StepIcon->getID(), ImVec2{ size, size })) // Step
			{
				if(m_Scene->IsPaused())
					m_Scene->StepFrames(60);
			}
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::End();
	}

	void EditorLayer::PlayScene()
	{
		// TempScene Is Another Reference To the same object
		m_TempScene = m_Scene;
		m_CurrentSceneState = SceneState::PLAY;

		if(!m_KeepRunTimeChanges)
			m_Scene = Scene::Copy(m_TempScene);
		m_Scene->OnRunTimeStart();

		m_SceneHierarchyPanel.SetScene(m_Scene); // Added To Modify Scene At RunTime From inspector
	}

	void EditorLayer::StopScene()
	{
		m_CurrentSceneState = SceneState::EDIT;
		m_Scene->OnRunTimeStop();

		if (!m_KeepRunTimeChanges)
			m_Scene = m_TempScene;

		m_SceneHierarchyPanel.SetScene(m_Scene); // Added To Modify Scene At RunTime From inspector
	}

	void EditorLayer::PlaySimulateScene()
	{
		// TempScene Is Another Reference To the same object
		m_TempScene = m_Scene;
		m_CurrentSceneState = SceneState::Simulate;

		if (!m_KeepRunTimeChanges)
			m_Scene = Scene::Copy(m_TempScene);
		m_Scene->OnSimulationStart(); // Start 2D Physics Setup

		m_SceneHierarchyPanel.SetScene(m_Scene); // Added To Modify Scene At RunTime From inspector
	}

	void EditorLayer::StopSimulationScene()
	{
		m_CurrentSceneState = SceneState::EDIT;
		m_Scene->OnSimulationStop(); // Stop 2D Physics

		if (!m_KeepRunTimeChanges)
			m_Scene = m_TempScene;

		m_SceneHierarchyPanel.SetScene(m_Scene); // Added To Modify Scene At RunTime From inspector
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_CurrentSceneState != SceneState::EDIT)
			return;

		Entity SelctedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		m_Scene->DuplicateEntity(SelctedEntity);
	}

	void EditorLayer::RenderOverLayer()
	{
		Renderer2D::RendererCamera rcam;
		if (m_CurrentSceneState == SceneState::PLAY)
		{
			if (auto& maincamera = m_Scene->GetMainCameraEntity())
			{
				auto& CamTransform = maincamera.GetComponent<TransformComponent>();
				auto& cam = maincamera.GetComponent<CameraComponent>().m_Camera;

				rcam.Projection = cam.GetProjectionMatrix();
				rcam.View = glm::inverse(CamTransform.GetTransformMatrix());
			}
		}
		else
		{
			rcam.Projection = m_EditorCamera.GetProjectionMatrix();
			rcam.View = m_EditorCamera.GetViewMatrix();
		}

		Renderer2D::Begin(rcam);
		
		{ // BoxCollider
			auto group = m_Scene->GetRegistry().view<TransformComponent, BoxCollider2DComponent>();

			for (auto entity : group)
			{
				auto [transform, collider] = group.get<TransformComponent, BoxCollider2DComponent>(entity);

				glm::vec3 Position =
				{
				  transform.m_Position.x + collider.m_Offset.x,
				  transform.m_Position.y + collider.m_Offset.y,
				  transform.m_Position.z + 0.01f
				};

				glm::vec3 Scale =
				{
				  transform.m_Scale.x * collider.m_Size.x * 2.0f,
				  transform.m_Scale.y * collider.m_Size.y * 2.0f,
				  1.0f
				};

				Scale.x += 0.01f;
				Scale.y += 0.01f;

				// Exclude Roation form original transform
				glm::mat4 Transform =
					glm::translate(glm::mat4(1.0f), Position) *
					glm::rotate(glm::mat4(1.0f), glm::radians(transform.m_Rotation.z), {0, 0, 1}) *
					glm::scale(glm::mat4(1.0f), Scale);

				Renderer2D::SetLineWidth(3.0f);
				Renderer2D::DrawRect(Transform, { 0.0f, 1.0f, 0.0f, 1.0f });
			}
		}

		{ // CircleCollider
			auto group = m_Scene->GetRegistry().view<TransformComponent, CircleCollider2DComponent>();

			for (auto entity : group)
			{
				auto [transform, collider] = group.get<TransformComponent, CircleCollider2DComponent>(entity);

				float ColliderThickness = 0.05f;

				glm::vec2 Position = glm::vec2(transform.m_Position) + collider.m_Offset;
				glm::vec3 Scale = transform.m_Scale * glm::vec3(collider.m_Raduis * 2.0f);
				Scale += ColliderThickness;

				// Exclude Roation form original transform
				glm::mat4 Transform =
					glm::translate(glm::mat4(1.0f), { Position.x , Position.y, transform.m_Position.z }) *
					glm::scale(glm::mat4(1.0f), { Scale.x, Scale.y, 1.0f });

				Renderer2D::DrawCircle(Transform, { 0.0f, 1.0f, 0.0f, 1.0f }, ColliderThickness);
			}
		}


		Renderer2D::End();
	}


























	Application* CreatApplication()
	{
		return new SnapEditor();
	}
}