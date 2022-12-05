#pragma once
#include <Snap/Scene/Entity.h>
#include <Snap/Core/TimeStep.h>

namespace SnapEngine
{
	class CppScript
	{
	public:
		virtual ~CppScript() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}


		virtual void Start() {}
		virtual void Update(TimeStep Time) {}
		virtual void Destroy() {}


		Entity m_Entity;
		friend class Scene;
	};
}