#pragma once
#include <Snap/Scene/Entity.h>
#include <Snap/Core/TimeStep.h>
#include <Snap/Events/Event.h>

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

		template<typename T>
		bool HasComponent()
		{
			return m_Entity.HasComponent<T>();
		}


		virtual void Start() {}
		virtual void Update(TimeStep Time) {}
		virtual void Destroy() {}
		virtual void ProcessEvents(IEvent& e) {}


		Entity m_Entity;
		friend class Scene;
	};
}