#include "SnapPCH.h"
#include "EntityManager.h"

namespace enttt
{
	EntityManager::EntityManager(uint32_t MaxEntityCount, uint32_t MaxComponentCountPerEntity)
		: m_MaxEntityCount(MaxEntityCount), m_MaxComponentCount(MaxComponentCountPerEntity)
	{
		m_Entities.resize(m_MaxEntityCount, (uint32_t)-1);
		ComponentList.resize(m_MaxEntityCount * m_MaxComponentCount);
		ComponentSet.resize(m_MaxEntityCount * m_MaxComponentCount);
	}

	Entity EntityManager::CreatEntity()
	{
		Entity entity = Current_Entity_ID++;
		m_Entities[entity] = entity;
		return entity;
	}
}