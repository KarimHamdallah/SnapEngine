#pragma once
#include "Event.h"
#include "SnapPCH.h"

namespace SnapEngine
{
	class WindowResizeEvent : public IEvent
	{
	public:
		WindowResizeEvent(unsigned int Width, unsigned int Height)
			: m_Width(Width), m_Height(Height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent : " << m_Width << " , " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)

	private:
		unsigned int m_Width, m_Height;
	};

	class WindowCloseEvent : public IEvent
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryApplication)
	};
}