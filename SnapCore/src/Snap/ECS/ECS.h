#pragma once
#include <Snap/Core/asserts.h>
#include "Component.h"


using ComponentTypeID = uint32_t;

inline ComponentTypeID GetUniqueID()
{
	static ComponentTypeID UniqueID = 0u;
	return UniqueID++;
}

template<typename T>
inline ComponentTypeID GetComponentTypeID()
{
	SNAP_ASSERT_MSG(static_cast<bool>(std::is_base_of<Component, T>::value), "Type is no a component!");
	static const ComponentTypeID TypeID = GetUniqueID();
	return TypeID;
}