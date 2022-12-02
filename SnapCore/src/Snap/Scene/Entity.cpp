#include "SnapPCH.h"
#include "Entity.h"


namespace SnapEngine
{
	Entity::Entity(Handel handel, Scene* scene)
		: entity(handel), mScene(scene)
	{}

	Entity::operator bool() const
	{
		return entity != entt::null;
	}

	Entity::operator Handel() const
	{
		return entity;
	}

	Entity::operator uint32_t() const
	{
		return static_cast<uint32_t>(entity);
	}

	bool Entity::operator ==(const Entity& other) const
	{
		return entity == other.entity && mScene == other.mScene;
	}

	bool Entity::operator !=(const Entity& other) const
	{
		return entity != other.entity || mScene != other.mScene;
	}
}