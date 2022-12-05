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

				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<TransformComponent>())
		{

		}
	}
}