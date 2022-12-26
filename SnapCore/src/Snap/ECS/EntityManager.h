#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Core/asserts.h>
#include <Snap/ECS/ECS.h>

namespace enttt
{
	using Entity = uint32_t;

	class EntityManager
	{
	public:
		EntityManager(uint32_t MaxEntityCount, uint32_t MaxComponentCountPerEntity);
		virtual ~EntityManager() {}

		Entity CreatEntity();


		template<typename T>
		inline std::vector<uint32_t> View() const
		{
			std::vector<uint32_t> IDs;

			for (uint32_t entity = 0; entity < Current_Entity_ID; entity++)
			{
				if (this->HasComponent<T>(entity))
					IDs.push_back(entity);
			}

			return IDs;
		}

		template<typename T1, typename T2>
		inline std::vector<uint32_t> View() const
		{
			std::vector<uint32_t> IDs;

			for (uint32_t entity = 0; entity < Current_Entity_ID; entity++)
			{
				if (this->HasComponent<T1>(entity) && this->HasComponent<T2>(entity))
					IDs.push_back(entity);
			}

			return IDs;
		}

		template<typename T, typename Func>
		inline void Each(Func func)
		{
			std::vector<uint32_t> IDs;

			for (uint32_t entity = 0; entity < Current_Entity_ID; entity++)
			{
				if (this->HasComponent<T>(entity))
					IDs.push_back(entity);
			}

			for (uint32_t entityID : IDs)
				func(entityID, this->GetComponent<T>(entityID));
		}

		template<typename T, typename... TArgs>
		inline T& AddComponent(Entity entity, TArgs&&... args)
		{
			SNAP_ASSERT_MSG(!this->HasComponent<T>(entity), "Entity already Has Component!");

			T* component = new T(std::forward<TArgs>(args)...); // Creat Component Object
			//SnapEngine::SnapUniquePtr<T> CompPtr = (SnapEngine::SnapUniquePtr<T>)std::unique_ptr<T>(component); // Creat UniquePtr

			if (component->Init()) // Init Component
			{
				uint32_t ComponentIndexPos = GetComponentTypeID<T>() + entity * m_MaxComponentCount;
				ComponentList[ComponentIndexPos] = component;
				ComponentSet[ComponentIndexPos] = true;
				return *component;
			}

			return *static_cast<T*>(nullptr);
		}

		template<typename T>
		inline T& GetComponent(Entity entity) const
		{
			SNAP_ASSERT_MSG(this->HasComponent<T>(entity), "Entity not Has Component Type Required!");

			uint32_t ComponentIndexPos = GetComponentTypeID<T>() + entity * m_MaxComponentCount;
			auto CompPtr = ComponentList[ComponentIndexPos];
			return *static_cast<T*>(CompPtr);
		}

		template<typename T>
		inline bool HasComponent(Entity entity) const
		{
			SNAP_ASSERT_MSG(this->IsComponent<T>(), "Type Passed Is not a Component!");
			uint32_t ComponentIndexPos = GetComponentTypeID<T>() + entity * m_MaxComponentCount;
			return ComponentSet[ComponentIndexPos];
		}

		template<typename T>
		inline bool IsComponent() const { return static_cast<bool>(std::is_base_of<Component, T>::value); }

	private:
		std::vector<Entity> m_Entities;
		std::vector<Component*> ComponentList; // Components present at it's TypeID index in list
		std::vector<bool> ComponentSet; // Components present at it's TypeID index in set as a bool


		uint32_t m_MaxEntityCount = 0u;
		uint32_t m_MaxComponentCount = 0u;

		uint32_t Current_Entity_ID = 0u;
	};
}