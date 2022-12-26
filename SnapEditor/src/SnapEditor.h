#pragma once

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


			/// Scene ///////////////////////////////////////////////

			m_Scene = CreatSnapPtr<Scene>();
			m_SceneHierarchyPanel.SetScene(m_Scene);
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

		}

		void ImGuiRender() override
		{
			StartDockSpace();

			m_SceneHierarchyPanel.ImGuiRender();
			m_ContentBrowserPanel.ImGuiRender();



			ImGui::Begin("Settings");
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
				const auto* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM");
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

			if (SelectedEntity)
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
		/////////////////////////////////////////////////////////
	

private:
		void StartDockSpace();
		void EndDockSpace();

		void OpenScene();
		void NewScene();
		void SaveScene();

		virtual void ProcessEvent(IEvent& e) override;
		
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMousePressed(MousePressedEvent& e);
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