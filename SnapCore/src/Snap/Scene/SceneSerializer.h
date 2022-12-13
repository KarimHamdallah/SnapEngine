#pragma once
#include <snap/Scene/Scene.h>
#include <Snap/Scene/Entity.h>
#include <Snap/Scene/Comps/Components.h>

namespace SnapEngine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const SnapPtr<Scene>& scene);
		~SceneSerializer() {}

		void SerializeScene(const std::string& filepath);

		bool DeSerializeScene(const std::string& filepath);

	private:
		SnapPtr<Scene> m_Scene;
	};
}