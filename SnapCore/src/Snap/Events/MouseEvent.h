#pragma once
#include "Event.h"
#include "SnapPCH.h"

namespace SnapEngine
{
	class MouseButtonEvent : public IEvent
	{
	public:
		MouseButtonEvent(int Button)
			: m_Button(Button) {}

		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryInput | (int)EventCategory::EventCategoryMouseButton)
	protected:
		int m_Button;
	};

	class MousePressedEvent : public MouseButtonEvent
	{
	public:
		MousePressedEvent(int Button)
			: MouseButtonEvent(Button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MousePressedEvent : MouseButton : " << m_Button;
			return ss.str();
		}

		inline int GetMouseButton() { return m_Button; }

		EVENT_CLASS_TYPE(MousePressed)
	};

	class MouseReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseReleasedEvent(int Button)
			: MouseButtonEvent(Button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseReleasedEvent : MouseButton : " << m_Button;
			return ss.str();
		}

		inline int GetMouseButton() { return m_Button; }

		EVENT_CLASS_TYPE(MouseReleased)
	};

	class MouseMovedEvent : public IEvent
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_X(x), m_Y(y) {}

		inline float GetX() const { return m_X; }
		inline float GetY() const { return m_Y; }


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent : MouseCoord : " << m_X << " , " << m_Y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryInput | (int)EventCategory::EventCategoryMouse)

	private:
		float m_X, m_Y = 0;
	};

	class MouseScrollEvent : public IEvent
	{
	public:
		MouseScrollEvent(float x_offset, float y_offset)
			: m_OffsetX(x_offset), m_OffsetY(y_offset) {}

		inline float GetOffsetX() const { return m_OffsetX; }
		inline float GetOffsetY() const { return m_OffsetY; }


		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrollEvent : MouseScrollOffset : " << m_OffsetX << " , " << m_OffsetY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScroll)
		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryInput | (int)EventCategory::EventCategoryMouse)

	private:
		float m_OffsetX, m_OffsetY = 0;
	};
}