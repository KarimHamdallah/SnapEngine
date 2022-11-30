#pragma once
#include <Snap/Core/Core.h>
#include "SnapPCH.h"

namespace SnapEngine
{
	enum class EventType
	{
		WindowResize, WindowClose,
		AppStart, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseMoved, MouseScroll, MousePressed, MouseReleased
	};

	enum class EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyBoard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
                               virtual const char* GetName() const override { return #type; }\
                               virtual EventType GetEventType() const override { return GetStaticType(); }


#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class IEvent // Event Interface
	{
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0; // WARNING:: For debug purpos only
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); } // WARNING:: For debug purpos only

		inline bool IsInCategory(EventCategory e_cat)
		{
			return GetCategoryFlags() & (int)e_cat;
		}


		friend class EventDispatcher;
		bool m_Handeled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventDispatchFuncPtr = std::function<bool(T&)>; // Function Should Run To Process Event

	public:
		EventDispatcher(IEvent& event)
			: m_Event(event) {}

		template<typename T>
		bool DispatchEvent(EventDispatchFuncPtr<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handeled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		IEvent& m_Event;
	};

	inline std::ostream& operator << (std::ostream& os, const IEvent& e)
	{
		return os << e.ToString();
	}
}