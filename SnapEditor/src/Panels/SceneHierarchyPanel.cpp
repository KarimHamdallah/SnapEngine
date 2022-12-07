#include "SceneHierarchyPanel.h"
#include <Snap/Scene/Comps/Components.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace SnapEngine
{
	SceneHierarchyPanel::SceneHierarchyPanel(const SnapPtr<Scene>& scene)
	{
		m_Scene = scene;
	}

	void SceneHierarchyPanel::SetScene(const SnapPtr<Scene>& scene)
	{
		m_Scene = scene;
	}

	void SceneHierarchyPanel::ImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Scene->GetRegistry().each([=](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		ImGui::End();


		ImGui::Begin("Inspector");
		
		if(m_SelectedEntity)
			DrawComponents(m_SelectedEntity);
		
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(const Entity& entity)
	{
		auto& tag = m_Scene->GetRegistry().get<TagComponent>(entity).m_Tag;

		ImGuiTreeNodeFlags flags =  ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		if (opened)
			ImGui::TreePop();
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			ImGui::Separator();

			auto& Tag = entity.GetComponent<TagComponent>().m_Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), Tag.c_str());

			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
				Tag = std::string(buffer);
		}

		if (entity.HasComponent<TransformComponent>())
		{
			ImGui::Separator();

			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(),
				ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto& transform = entity.GetComponent<TransformComponent>().m_Transform;
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.01f);
				/*
				auto& transform = entity.GetComponent<TransformComponent>();
				ImGui::DragFloat3("Position", glm::value_ptr(transform.m_Position), 0.01f);
				ImGui::DragFloat3("Rotation", glm::value_ptr(transform.m_Rotation), 0.01f);
				ImGui::DragFloat3("Scale", glm::value_ptr(transform.m_Scale), 0.01f);
				*/
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<CameraComponent>())
		{
			ImGui::Separator();

			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(),
				ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				bool Changed = false;
				auto& CamComp = entity.GetComponent<CameraComponent>();
				
				int CamType = (int)CamComp.m_Camera.GetProjectionType();
				const char* CameraProjectionTypeString[2] = { "Perspective", "OrthoGraphic" };
				const char* CurrentProjectionTypeString = CameraProjectionTypeString[CamType];

				if (ImGui::BeginCombo("Camera Projection", CurrentProjectionTypeString))
				{
					for (size_t i = 0; i < 2; i++)
					{
						bool IsSelected = CurrentProjectionTypeString == CameraProjectionTypeString[i];
						if (ImGui::Selectable(CameraProjectionTypeString[i], IsSelected)
							&& CurrentProjectionTypeString != CameraProjectionTypeString[i])
						{
							// Change Projection To new Projection
							CurrentProjectionTypeString = CameraProjectionTypeString[i];
							CamComp.m_Camera.SetProjectionType((SceneCamera::ProjectionType)i);
							Changed = true;
						}

						if (IsSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				// Draw Camera UI Parameters
				if (CamComp.m_Camera.GetProjectionType() == SceneCamera::ProjectionType::OrthoGraphic)
				{
					float Size = CamComp.m_Camera.GetOrthoGraphicSize();
					float NearClipPlane = CamComp.m_Camera.GetOrthoGraphicNear();
					float FarClipPlane = CamComp.m_Camera.GetOrthoGraphicFar();

					if (Changed)
					{
						CamComp.m_Camera.SetOrthoGraphic(Size, NearClipPlane, FarClipPlane);
						Changed = false;
					}

					if (ImGui::DragFloat("Size", &Size, 0.01f))
						CamComp.m_Camera.SetOrthoGraphicSize(Size);
					if (ImGui::DragFloat("NearClip", &NearClipPlane, 0.01f))
						CamComp.m_Camera.SetOrthoGraphicNear(NearClipPlane);
					if (ImGui::DragFloat("FarClip", &FarClipPlane, 0.01f))
						CamComp.m_Camera.SetOrthoGraphicFar(FarClipPlane);
				}

				if (CamComp.m_Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float Fov = glm::degrees(CamComp.m_Camera.GetPerspectiveFov());
					float NearClipPlane = CamComp.m_Camera.GetPerspectiveNear();
					float FarClipPlane = CamComp.m_Camera.GetPerspectiveFar();

					if (Changed)
					{
						CamComp.m_Camera.SetPerspective(glm::radians(Fov), NearClipPlane, FarClipPlane);
						Changed = false;
					}

					if (ImGui::DragFloat("Fov", &Fov, 0.01f))
						CamComp.m_Camera.SetPerspectiveFov(glm::radians(Fov));
					if (ImGui::DragFloat("NearClip", &NearClipPlane, 0.01f))
						CamComp.m_Camera.SetPerspectiveNear(NearClipPlane);
					if (ImGui::DragFloat("FarClip", &FarClipPlane, 0.01f))
						CamComp.m_Camera.SetPerspectiveFar(FarClipPlane);
				}

				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			ImGui::Separator();

			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(),
				ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
			{

				auto& SpriteRendererComp = entity.GetComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(SpriteRendererComp.m_Color));

				ImGui::TreePop();
			}
		}
	}
}