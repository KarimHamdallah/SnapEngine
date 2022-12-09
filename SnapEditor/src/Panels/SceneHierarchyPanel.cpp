#include "SceneHierarchyPanel.h"
#include <Snap/Scene/Comps/Components.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>

namespace SnapEngine
{
	static void DrawVec3Control(const std::string& lable, glm::vec3& value, float resetvalue = 0.0f,float Speed = 0.01f, float colwidth = 80.0f)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImFont* BoldFont = io.Fonts->Fonts[0];

		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colwidth);
		ImGui::Text(lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 ButtonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.7f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.4f, 0.4f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("X", ButtonSize))
			value.x = resetvalue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &value.x, Speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.15f, 0.8f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.4f, 0.9f, 0.4f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.8f, 0.15f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("Y", ButtonSize))
			value.y = resetvalue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &value.y, Speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.15f, 0.1f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.4f, 0.4f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.1f, 0.8f, 1.0f });
		ImGui::PushFont(BoldFont);
		if (ImGui::Button("Z", ButtonSize))
			value.z = resetvalue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &value.z, Speed);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction UIfunction)
	{
		auto ComponentNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
			| ImGuiTreeNodeFlags_OpenOnDoubleClick
			| ImGuiTreeNodeFlags_DefaultOpen;

		if (entity.HasComponent<T>())
		{
			T& component = entity.GetComponent<T>(); // Get Component

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4, 4 });
			
			// creat Componnet Node With (ComponnetNodeFlags) Props and Name (name)
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), ComponentNodeFlags, name.c_str());
			
			// Draw Component Properites Menu
			ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
			if (ImGui::Button("+", { 20.0f, 20.0f }))
				ImGui::OpenPopup("Component Settings");
			
			ImGui::PopStyleVar();

			// Component Properties
			bool removed = false;
			if (ImGui::BeginPopup("Component Settings"))
			{
				if (ImGui::MenuItem("Remove Component"))
					removed = true;

				ImGui::EndPopup();
			}

			// Draw Component Features On Node Open
			if (open)
			{
				UIfunction(component); // Draw Component UI

				ImGui::TreePop(); // PopTree On Node Open
			}

			// Remove Component On Component Removed
			if (removed)
				entity.RemoveComponent<T>();
			
			ImGui::Separator(); // Separator Between each 2 components
		}
	}

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

		// Left Click on Balnk Space
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectedEntity = {};

		// Right Click on Balnk Space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Creat Empty"))
				m_Scene->CreatEntity("Empty");
			
			ImGui::EndPopup();
		}

		ImGui::End();


		ImGui::Begin("Inspector");
		
		if (m_SelectedEntity)
		{
			DrawComponents(m_SelectedEntity);

			ImGui::Separator();
			if (ImGui::Button("Add Component", {150.0f, 25.0f}))
				ImGui::OpenPopup("Add Component");

			if (ImGui::BeginPopup("Add Component"))
			{
				if (ImGui::MenuItem("Tag Component"))
					if (!m_SelectedEntity.HasComponent<TagComponent>())
						m_SelectedEntity.AddComponent<TagComponent>();
					else
						SNAP_WARN("Entity With Tag {0}, Already Has Tag Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);

				if (ImGui::MenuItem("Transform Component"))
					if (!m_SelectedEntity.HasComponent<TransformComponent>())
						m_SelectedEntity.AddComponent<TransformComponent>();
					else
						SNAP_WARN("Entity With Tag {0}, Already Has Transform Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);

				if (ImGui::MenuItem("Camera Component"))
					if (!m_SelectedEntity.HasComponent<CameraComponent>())
						m_SelectedEntity.AddComponent<CameraComponent>(10.0f, -100.0f, 100.0f).m_IsMain = true;
					else
						SNAP_WARN("Entity With Tag {0}, Already Has Camera Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);

				if (ImGui::MenuItem("SpriteRenderer Component"))
					if (!m_SelectedEntity.HasComponent<SpriteRendererComponent>())
						m_SelectedEntity.AddComponent<SpriteRendererComponent>();
					else
						SNAP_WARN("Entity With Tag {0}, Already Has SpriteRenderer Component!", m_SelectedEntity.GetComponent<TagComponent>().m_Tag);
				

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(const Entity& entity)
	{
		auto& tag = m_Scene->GetRegistry().get<TagComponent>(entity).m_Tag;

		ImGuiTreeNodeFlags flags =  ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
			m_SelectedEntity = entity;

		// Right Click on Entity Node
		bool entitydestroyed = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entitydestroyed = true;

			ImGui::EndPopup();
		}

		if (opened)
			ImGui::TreePop();

		if (entitydestroyed)
		{
			m_Scene->DestroyEntity(entity);
			if (m_SelectedEntity == entity)
				m_SelectedEntity = {};
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			ImGui::Separator();

			auto& Tag = entity.GetComponent<TagComponent>().m_Tag; // Get Tag
			
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), Tag.c_str()); // Copy Tag Content To Buffer
			
			if (ImGui::InputText("Tag", buffer, sizeof(buffer))) // Draw Buffer Into InputText
				Tag = std::string(buffer);
		}

		DrawComponent<TransformComponent>("Transform", entity, [&](TransformComponent& Component)
			{
				DrawVec3Control("Position", Component.m_Position);
				DrawVec3Control("Rotation", Component.m_Rotation, 0.0f, 0.1f);
				DrawVec3Control("Scale", Component.m_Scale);
			});

		DrawComponent<CameraComponent>("Camera", entity, [&](CameraComponent& Component)
			{
				bool Changed = false;

				int CamType = (int)Component.m_Camera.GetProjectionType();
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
							Component.m_Camera.SetProjectionType((SceneCamera::ProjectionType)i);
							Changed = true;
						}

						if (IsSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				// Draw Camera UI Parameters
				if (Component.m_Camera.GetProjectionType() == SceneCamera::ProjectionType::OrthoGraphic)
				{
					float Size = Component.m_Camera.GetOrthoGraphicSize();
					float NearClipPlane = Component.m_Camera.GetOrthoGraphicNear();
					float FarClipPlane = Component.m_Camera.GetOrthoGraphicFar();

					if (Changed)
					{
						Component.m_Camera.SetOrthoGraphic(Size, NearClipPlane, FarClipPlane);
						Changed = false;
					}

					if (ImGui::DragFloat("Size", &Size, 0.01f))
						Component.m_Camera.SetOrthoGraphicSize(Size);
					if (ImGui::DragFloat("NearClip", &NearClipPlane, 0.01f))
						Component.m_Camera.SetOrthoGraphicNear(NearClipPlane);
					if (ImGui::DragFloat("FarClip", &FarClipPlane, 0.01f))
						Component.m_Camera.SetOrthoGraphicFar(FarClipPlane);
				}
				else if (Component.m_Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float Fov = glm::degrees(Component.m_Camera.GetPerspectiveFov());
					float NearClipPlane = Component.m_Camera.GetPerspectiveNear();
					float FarClipPlane = Component.m_Camera.GetPerspectiveFar();

					if (Changed)
					{
						Component.m_Camera.SetPerspective(glm::radians(Fov), NearClipPlane, FarClipPlane);
						Changed = false;
					}

					if (ImGui::DragFloat("Fov", &Fov, 0.01f))
						Component.m_Camera.SetPerspectiveFov(glm::radians(Fov));
					if (ImGui::DragFloat("NearClip", &NearClipPlane, 0.01f))
						Component.m_Camera.SetPerspectiveNear(NearClipPlane);
					if (ImGui::DragFloat("FarClip", &FarClipPlane, 0.01f))
						Component.m_Camera.SetPerspectiveFar(FarClipPlane);
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [&](SpriteRendererComponent& Component)
			{
				glm::vec4& Color = Component.m_Color;
				ImGui::ColorEdit4("Color", glm::value_ptr(Color));
			});
	}
}