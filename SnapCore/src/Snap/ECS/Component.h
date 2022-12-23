#pragma once
class Entity;

class Component
{
public:
	Component() = default;
	virtual ~Component() {}

	virtual bool Init() { return true; }
	virtual void Update() {}

	inline void SetEntity(Entity* entity) { m_Entity = entity; }
	inline Entity* GetEntity() { return m_Entity; }

private:
	Entity* m_Entity = nullptr;
};