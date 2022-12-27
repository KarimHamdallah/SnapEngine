#include "SnapPCH.h"
#include "EntityManager.h"

namespace enttt
{
	Entity EntityManager::CreatEntity()
	{
		Entity entity = Current_Entity_ID++;
		m_EntityList[entity];
		return entity;
	}
}