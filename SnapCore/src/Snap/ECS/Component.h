#pragma once

class Component
{
public:
	Component() = default;
	virtual ~Component() {}

	virtual bool Init() { return true; }
	virtual void Update() {}
private:
};