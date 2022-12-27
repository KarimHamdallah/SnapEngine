#pragma once
#include <Snap/Core/Core.h>
#include <Snap/Core/asserts.h>
#include <Snap/ECS/ECS.h>
#include <set>

namespace enttt
{
	using Entity = uint32_t;
	using CompsMap = std::map<uint32_t, SnapEngine::SnapPtr<Component>>;

	class EntityManager
	{
	public:
		EntityManager() = default;
		virtual ~EntityManager() {}

		Entity CreatEntity();

		
		template<typename T>
		inline const std::vector<Entity>& View() const
		{
			return m_Views.at(GetComponentTypeID<T>());
		}

		template<typename T1, typename T2>
		inline std::vector<Entity> View() const
		{
			std::vector<Entity> group1, group2;
			
			std::vector<Entity> Set;

			auto& it1 = m_Views.find(GetComponentTypeID<T1>());
			auto& it2 = m_Views.find(GetComponentTypeID<T2>());
			if (it1 != m_Views.end())
				group1 = it1->second;
			if (it2 != m_Views.end())
				group2 = it2->second;

			uint32_t Length = group1.size() < group2.size() ? group1.size() : group2.size();
			Set.resize(Length);

			for (size_t i = 0; i < Length; i++)
				if (group1[i] == group2[i])
					Set[i] = group1[i];
			
			return Set;
		}

		
		template<typename T, typename Func>
		inline void Each(Func func)
		{
			std::vector<uint32_t> IDs = View<T>();

			for (uint32_t entityID : IDs)
				func(entityID, this->GetComponent<T>(entityID));
		}


		template<typename T, typename... TArgs>
		inline T& AddComponent(Entity entity, TArgs&&... args)
		{
			SNAP_ASSERT_MSG(!this->HasComponent<T>(entity), "Entity already Has Component!");

			SnapEngine::SnapPtr<T> component = SnapEngine::CreatSnapPtr<T>(std::forward<TArgs>(args)...); // Creat Component Object

			if (component->Init()) // Init Component
			{
				m_EntityList[entity][GetComponentTypeID<T>()] = component;
				
				
				auto pair = m_Views.find(GetComponentTypeID<T>());
				if (pair != m_Views.end()) // if pair is valid
					pair->second.push_back(entity);
				else
					m_Views[GetComponentTypeID<T>()] = std::vector<Entity>(1, entity); // Add Value To Key

				return *component;
			}

			return *static_cast<T*>(nullptr);
		}

		template<typename T>
		inline T& GetComponent(Entity entity) const
		{
			SNAP_ASSERT_MSG(this->HasComponent<T>(entity), "Entity not Has Component Type Required!");	
			
			const CompsMap& ComponentsMap = m_EntityList.at(entity);
			auto CompPtr = ComponentsMap.at(GetComponentTypeID<T>());
			return *static_cast<T*>(CompPtr.get());
		}

		template<typename T>
		inline bool HasComponent(Entity entity) const
		{
			SNAP_ASSERT_MSG(this->IsComponent<T>(), "Type Passed Is not a Component!");

			const CompsMap& ComponentsMap = m_EntityList.at(entity);
			return ComponentsMap.find(GetComponentTypeID<T>()) != ComponentsMap.end();
		}

		template<typename T>
		inline bool IsComponent() const { return static_cast<bool>(std::is_base_of<Component, T>::value); }

	private:
		std::map<Entity, std::map<ComponentTypeID, SnapEngine::SnapPtr<Component>>> m_EntityList;
		std::map <ComponentTypeID, std::vector<Entity>> m_Views; // for each componentTypeID we have list of enttites have it

		uint32_t Current_Entity_ID = 0u;
	};
}