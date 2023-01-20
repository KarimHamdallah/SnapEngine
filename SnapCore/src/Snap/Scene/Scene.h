#pragma once
#include <entt.hpp>
#include <glm/glm.hpp>
#include <Snap/Core/Core.h>
#include <Snap/Core/UUID.h>

#include <Snap/Scene/EditorCamera.h>

#define MAX_ENTITIES 10000

class b2World;

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

		Entity& CreatEntityWithUUID(
			UUID uuid,
			const std::string& name = std::string(),
			const glm::vec3& Position = glm::vec3(0.0f),
			const glm::vec3& Scale = glm::vec3(1.0f),
			const glm::vec3& Rotation = glm::vec3(0.0f)
		);

		static SnapPtr<Scene> Copy(const SnapPtr<Scene>& other);
		
		void DestroyEntity(Entity entity);
		void DuplicateEntity(Entity entity);


		const Resgistry& GetRegistry() const noexcept { return registry; }

		void UpdateEditor(const TimeStep& Time, const EditorCamera& Camera);
		void UpdateRunTime(const TimeStep& Time);
		void RunTimeRender();

		void UpdateAndRenderSimulation(const TimeStep& Time, const EditorCamera& Camera);


		// Editor Functions
		void ResizeViewPort(uint32_t Width, uint32_t Height); // Update All Scene Cameras Projection According To ViewPort Size Changes

		void ProcessEvents(IEvent* e);

		Entity GetMainCameraEntity();

		void OnRunTimeStart(); // Called When play button clicked
		void OnRunTimeStop(); // Called When Stop button clicked

		void OnSimulationStart(); // Called When Simulate button clicked
		void OnSimulationStop(); // Called When Stop Simulation button clicked

		Entity FindEntityByName(std::string_view Name);

		Entity GetEntityWithUUID(UUID uuid);

		inline bool IsRunning() { return m_IsRunning; }
		inline bool IsPaused() { return m_IsPaused; }

		inline void SetPaused(bool paused) { m_IsPaused = paused; }
		inline void StepFrames(int frames = 1) { m_StepsNumber = frames; }

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);
	private:
		Resgistry registry;
		std::vector<SnapPtr<Entity>> m_Entities;
		inline static uint32_t EntityCounter = 0;
		friend Entity;

		void Physics2DStart();
		void Physics2DStop();

		bool m_IsRunning = false;
		bool m_IsPaused = false;
		int m_StepsNumber = 1;

		// Editor Attribs
		uint32_t m_ViewPortWidth = 0, m_ViewPortHeight = 0;

		b2World* m_PhysicsWorld = nullptr;
	};
}