#pragma once

namespace SnapEngine
{
	class TimeStep
	{
	public:
		TimeStep(float time = 0.0f)
			: m_Time(time)
		{}

		operator float() const { return m_Time; } // Cast to float or send as float to functions

		inline float GetSeconds() { return m_Time; }
		inline float GetMilliSeconds() { return m_Time * 1000.0f; }

	private:
		float m_Time = 0.0f;
	};
}