#include "SnapPCH.h"
#include "EntityManager.h"
#include <Snap/Core/asserts.h>

Entity& EntityManager::CreatEntity()
{
	Entity* entity = new Entity();
	entity->m_ID = Current_Entity_ID++;

	m_Entities[entity->m_ID] = SnapEngine::SnapUniquePtr<Entity>(entity);
	return *m_Entities[entity->m_ID];
}

Entity& EntityManager::GetEntity(uint32_t ID)
{
	SNAP_ASSERT_MSG(ID <= Current_Entity_ID, "Entity ID Is Out Of Range!");
	return *m_Entities[ID];
}