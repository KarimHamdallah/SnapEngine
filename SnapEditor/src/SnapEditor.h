#pragma once

#include <SnapEngine.h>
#include <Snap/Scene/Scripts/CameraControllerScript.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include <platform/Utils/PlatformUtils.h>

namespace SnapEngine
{
	extern const std::filesystem::path g_AssetPath;

	class EditorLayer : public SnapEngine::Layer
	{
	public:
		EditorLayer()
			: Layer("Example")
		{
			m_PlayIcon = SnapPtr<Texture2D>(Texture2D::Creat("assets/Editor/PlayButton.png"));
			m_StopIcon = SnapPtr<Texture2D>(Texture2D::Creat("assets/Editor/PauseButton.png"));
			m_SimulateIcon = SnapPtr<Texture2D>(Texture2D::Creat("assets/Editor/SimaulateButton.png"));


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


			/// Scene ///////////////////////////////////////////////

			m_Scene = CreatSnapPtr<Scene>();
			m_SceneHierarchyPanel.SetScene(m_Scene);


			////////////////////// Scripts //////////////////

			class SpriteScalerScript : public CppScript
			{
			public:
				virtual void Update(TimeStep Time) override
				{
					float Speed = 10.0f;
					if (HasComponent<CameraComponent>())
					{
						CameraComponent& cam = GetComponent<CameraComponent>();
						float CamSize = cam.m_Camera.GetOrthoGraphicSize();

						if (Input::IsKeyPressed(Key::Right))
							cam.m_Camera.SetOrthoGraphicSize(CamSize - Time * Speed);
						if (Input::IsKeyPressed(Key::Left))
							cam.m_Camera.SetOrthoGraphicSize(CamSize + Time * Speed);
					}
					else
					{
						TransformComponent& transform = GetComponent<TransformComponent>();
						if (Input::IsKeyPressed(Key::Right))
							transform.m_Scale.x += Time * Speed;
						if (Input::IsKeyPressed(Key::Left))
							transform.m_Scale.x -= Time * Speed;
						if (Input::IsKeyPressed(Key::Up))
							transform.m_Scale.y += Time * Speed;
						if (Input::IsKeyPressed(Key::Down))
							transform.m_Scale.y -= Time * Speed;
					}
				}
			};



			///////////// Scene Setup /////////////
			Entity Sprite = m_Scene->CreatEntity("Sprite");
			Sprite.AddComponent<SpriteRendererComponent>(glm::vec4(0.8f, 0.3f, 0.4f, 1.0f));
			Sprite.AddComponent<CppScriptComponent>().Bind<SpriteScalerScript>();

			Entity Camera = m_Scene->CreatEntity("Camera", glm::vec3(0.0f, 0.0f, 10.0f));
			Camera.AddComponent<CameraComponent>().m_IsMain = true;
			Camera.AddComponent<CppScriptComponent>().Bind<SpriteScalerScript>();
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
					m_Scene->ResizeViewPort(m_ViewPortSize.x, m_ViewPortSize.y);
					
					// Change Scene Cameras Projection According To New Width and New Height of ViewPortWindow
					m_EditorCamera.SetViewPortSize(m_ViewPortSize.x, m_ViewPortSize.y);
				}
			}

			// Render
			m_FrameBuffer->Bind(); // Record Scene into frame buffer

			Renderer2D::ResetStats();
			RendererCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
			RendererCommand::Clear();
			m_FrameBuffer->ClearAttachment(1, -1);

			switch (m_CurrentSceneState)
			{
			case SnapEngine::EditorLayer::SceneState::EDIT:
			{
				if (m_ViewPortFocused)
					m_EditorCamera.UpdateCamera(Time);
				
				m_Scene->UpdateEditor(Time, m_EditorCamera);// EditorRenderFunction

				// mouse picking
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
			}
				break;
			case SnapEngine::EditorLayer::SceneState::PLAY:
			{
				m_Scene->UpdateRunTime(Time);
				m_Scene->RunTimeRender(); // RunTimeRenderFunction
			}
				break;
			case SnapEngine::EditorLayer::SceneState::Simulate:
			{
				if (m_ViewPortFocused)
					m_EditorCamera.UpdateCamera(Time);
				m_Scene->UpdateAndRenderSimulation(Time, m_EditorCamera); // SimulateUpdateAndRenderFunction
			}
			break;
			default:
				break;
			}

			Renderer2D::End();

			if(m_VisualiseColliders)
				RenderOverLayer();

			m_FrameBuffer->UnBind(); // Stop Recording

		}

		void ImGuiRender() override
		{
			StartDockSpace();

			m_SceneHierarchyPanel.ImGuiRender();
			m_ContentBrowserPanel.ImGuiRender();
			UIToolbar();


			ImGui::Begin("Settings");
			ImGui::Checkbox("Keep RunTime Changes", &m_KeepRunTimeChanges);
			ImGui::Checkbox("Visualise Colliders",  &m_VisualiseColliders);
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

			if (ImGui::BeginDragDropTarget())
			{
				if (const auto* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					auto filepath = std::filesystem::path(g_AssetPath) / path;
					
					if(filepath.extension().string() == ".snap")
						OpenScene(filepath);
				}
				ImGui::EndDragDropTarget();
			}


			// Calculate ViewPort Window Bounds 
			auto WindowSize = ImGui::GetWindowSize();
			ImVec2 minBound = ImGui::GetWindowPos();

			minBound.x += ViewPortOffset.x;
			minBound.y += ViewPortOffset.y;

			ImVec2 maxBound = { minBound.x + WindowSize.x, minBound.y + WindowSize.y };
			m_ViewPortBounds[0] = { minBound.x, minBound.y };
			m_ViewPortBounds[1] = { maxBound.x, maxBound.y };



			// Draw ImGuizmo inside ViewPort Widnow
			Entity SelectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();

			if (SelectedEntity && m_CurrentSceneState == SceneState::EDIT)
			{
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist();
				float WindowPosX = ImGui::GetWindowPos().x;
				float WindowPosY = ImGui::GetWindowPos().y;
				float WindowWidth = ImGui::GetWindowWidth();
				float WindowHeight = ImGui::GetWindowHeight();
				ImGuizmo::SetRect(WindowPosX, WindowPosY, WindowWidth, WindowHeight);

				auto& e_transform = SelectedEntity.GetComponent<TransformComponent>();
				glm::mat4 EntityTransformMatrix = SelectedEntity.GetComponent<TransformComponent>().GetTransformMatrix();
				glm::mat4 ViewMatrix = m_EditorCamera.GetViewMatrix();
				glm::mat4 ProjectionMatrix = m_EditorCamera.GetProjectionMatrix();

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

			ImGui::End();
			ImGui::PopStyleVar();


			if (m_ContentBrowserPanel.IsWindowFocused() && m_ContentBrowserPanel.IsWindowHovered())
				Application::Get().GetImGuiLayer()->BlockEvents(false);

			EndDockSpace();
		}

	private:
		SnapPtr< FrameBuffer> m_FrameBuffer;
		SnapPtr<Scene> m_Scene;
		SnapPtr<Scene> m_TempScene; // hold Active Scene data before Playing it then reload TempScene Again

		/////////////////// ViewPort ////////////////////////////
		glm::vec2 m_ViewPortSize = { 800.0f, 600.0f }; // ImGui ViewPort Window Size
		bool m_ViewPortFocused = false;
		bool m_ViewPortHavored = false;

		glm::vec2 m_ViewPortBounds[2];
		/////////////////////////////////////////////////////////

		//////////////// Editor ///////////////
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowserPanel;
		int m_GizmoType = 0;
		float m_Snapping = 0.1f;
		EditorCamera m_EditorCamera;
		Entity m_HoveredEntity;
		bool m_KeepRunTimeChanges = false;
		bool m_VisualiseColliders = false;
		/////////////////////////////////////////////////////////



		enum class SceneState
		{
			PLAY = 0,
			EDIT = 1,
			Simulate = 3
		};

		SceneState m_CurrentSceneState = SceneState::EDIT;
		SnapPtr<Texture2D> m_PlayIcon, m_StopIcon, m_SimulateIcon;

private:
		void StartDockSpace();
		void EndDockSpace();

		void UIToolbar();

		void PlayScene();
		void StopScene();

		void PlaySimulateScene();
		void StopSimulationScene();

		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveScene(const std::filesystem::path& path);
		void NewScene();

		void OnDuplicateEntity();

		void RenderOverLayer();

		virtual void ProcessEvent(IEvent& e) override;
		
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MousePressedEvent& e);

		std::filesystem::path m_CurrentDeserializedScenePath = "";
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
}