#pragma once

namespace SnapEngine
{
	class Timer
	{
	public:
		Timer(const std::string& name)
			: m_Name(name)
		{
			m_StartTime = std::chrono::high_resolution_clock::now();
		}
		
		~Timer()
		{
			if (!Stopped)
				Stop();
		}

		void Stop()
		{

			auto& m_EndTime = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(m_EndTime).time_since_epoch().count();

			m_Duration = (end - start) * 0.001f; // duration im milliseconds
			Stopped = true;
		}

		float GetDuration() const { return m_Duration; } // duration im milliseconds
		std::string GetName() const { return m_Name; }
		
		operator float () const { return m_Duration; } // Pass as a float
		operator std::string() const { return m_Name; } // Pass as std::string

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
		bool Stopped = false;

		std::string m_Name;
		float m_Duration;
	};
}