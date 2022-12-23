#pragma once
#include <Snap/Core/Core.h>
#include "ECS.h"

class Entity
{
public:
	Entity() = default;
	virtual ~Entity() {}

	template<typename T, typename... TArgs>
	inline T& AddComponent(TArgs&&... args)
	{
		SNAP_ASSERT_MSG(!HasComponent<T>(), "Entity already Has Component!");

		T* component = new T(std::forward<TArgs>(args)...); // Creat Component Object
		SnapEngine::SnapUniquePtr<T> CompPtr = (SnapEngine::SnapUniquePtr<T>)std::unique_ptr<T>(component); // Creat UniquePtr
		Components.emplace_back(std::move(CompPtr)); // Add UniqePtr To Components Vector

		if (component->Init()) // Init Component
		{
			ComponentList[GetComponentTypeID<T>()] = component;
			ComponentSet[GetComponentTypeID<T>()] = true;
			component->SetEntity(this);
			return *component;
		}

		return *static_cast<T*>(nullptr);
	}

	template<typename T>
	inline T& GetComponent() const
	{
		SNAP_ASSERT_MSG(HasComponent<T>(), "Entity not Has Component Type Required!");
		auto CompPtr = ComponentList[GetComponentTypeID<T>()];
		return *static_cast<T*>(CompPtr);
	}

	template<typename T>
	inline bool HasComponent() const
	{
		SNAP_ASSERT_MSG(IsComponent<T>(), "Type Passed Is not a Component!");
		return ComponentSet[GetComponentTypeID<T>()];
	}

	template<typename T>
	inline bool IsComponent() const { return static_cast<bool>(std::is_base_of<Component, T>::value); }


private:
	std::array<Component*, MAX_COMPONENTS_COUNT> ComponentList; // Components present at it's TypeID index in list
	std::bitset<MAX_COMPONENTS_COUNT> ComponentSet; // Components present at it's TypeID index in set as a bool
	std::vector<SnapEngine::SnapPtr<Component>> Components; // Components present ordered by Adding in vector

	uint32_t m_ID;

	friend class EntityManager;
};