#pragma once
#include <Snap/Scene/Scene.h>
#include <Snap/Scene/Entity.h>

namespace SnapEngine
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const SnapPtr<Scene>& scene);

		void SetScene(const SnapPtr<Scene>& scene);
		Entity GetSelectedEntity() { return m_SelectedEntity; }

		void ImGuiRender();

	private:
		void DrawEntityNode(const Entity& entity);
		void DrawComponents(Entity entity);
	private:
		SnapPtr<Scene> m_Scene = nullptr;
		Entity m_SelectedEntity;
		uint32_t EmptyEntityCounter = 0;
	};
}