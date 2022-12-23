#pragma once
#include <Snap/ECS/Entity.h>

class EntityManager
{
public:
	EntityManager() = default;
	virtual ~EntityManager() {}

	Entity& CreatEntity();
	Entity& GetEntity(uint32_t ID);

	template<typename T>
	inline std::vector<uint32_t> View() const
	{
		std::vector<uint32_t> IDs;

		for (auto& entity : m_Entities)
		{
			if (entity.second->HasComponent<T>())
				IDs.push_back(entity.first);
		}

		return IDs;
	}

	template<typename T1, typename T2>
	inline std::vector<uint32_t> View() const
	{
		std::vector<uint32_t> IDs;

		for (auto& entity : m_Entities)
		{
			if (entity.second->HasComponent<T1>() && entity.second->HasComponent<T2>())
				IDs.push_back(entity.first);
		}

		return IDs;
	}

	template<typename T, typename Func>
	inline void Each(Func func)
	{
		std::vector<uint32_t> IDs;

		for (auto& entity : m_Entities)
		{
			if (entity.second->HasComponent<T>())
				IDs.push_back(entity.first);
		}

		for (uint32_t entityID : IDs)
			func(entityID, this->GetEntity(entityID).GetComponent<T>());
	}

private:
	std::map<uint32_t, SnapEngine::SnapUniquePtr<Entity>> m_Entities;
	uint32_t Current_Entity_ID = 0u;
};