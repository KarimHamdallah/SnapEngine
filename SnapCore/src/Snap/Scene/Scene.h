#pragma once
#include <entt.hpp>
#include <glm/glm.hpp>
#include <Snap/Core/Core.h>
#include <Snap/Core/TimeStep.h>
#include <Snap/Events/Event.h>

#define MAX_ENTITIES 10000

namespace SnapEngine
{
	class Entity;

	class Scene
	{
	public:
		Scene() { m_Entities.resize(MAX_ENTITIES); }
		using Resgistry = entt::registry;


		Entity& CreatEntity(
			const std::string& name = std::string(),
			const glm::vec3& Position = glm::vec3(0.0f),
			const glm::vec3& Scale = glm::vec3(1.0f),
			const glm::vec3& Rotation = glm::vec3(0.0f)
		);

		void DestroyEntity(Entity entity);

		const Resgistry& GetRegistry() const noexcept { return registry; }

		void Update(const TimeStep& Time);
		void Render();


		// Editor Functions
		void ResizeViewPort(uint32_t Width, uint32_t Height); // Update All Scene Cameras Projection According To ViewPort Size Changes

		void ProcessEvents(IEvent* e);

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		Resgistry registry;
		std::vector<SnapPtr<Entity>> m_Entities;
		inline static std::atomic_uint64_t EntityCounter = 0;
		friend Entity;


		// Editor Attribs
		uint32_t m_ViewPortWidth = 0, m_ViewPortHeight = 0;
	};
}