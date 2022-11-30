#pragma once
#include "Event.h"
#include "SnapPCH.h"

namespace SnapEngine
{
	class KeyEvent : public IEvent
	{
	public:
		KeyEvent(int KeyCode)
			: m_KeyCode(KeyCode) {}

		inline int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY((int)EventCategory::EventCategoryInput | (int)EventCategory::EventCategoryKeyBoard)
	protected:
		int m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int KeyCode, int RepeatCount)
			: KeyEvent(KeyCode), m_RepeatCount(RepeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent : KeyCode : " << m_KeyCode << " ( " << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int m_RepeatCount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int KeyCode)
			: KeyEvent(KeyCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent : KeyCode :" << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int KeyCode)
			: KeyEvent(KeyCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent : KeyCode : " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}