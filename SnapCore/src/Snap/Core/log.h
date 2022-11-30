#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace SnapEngine
{
	class Log
	{
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger> get_logger() { return m_logger; }
		static void free_memory();
	private:
		static std::shared_ptr<spdlog::logger> m_logger;
	};
}


#define SNAP_INFO(...)      SnapEngine::Log::get_logger()->info(__VA_ARGS__)
#define SNAP_DEBUG(...)     SnapEngine::Log::get_logger()->debug(__VA_ARGS__)
#define SNAP_TRACE(...)     SnapEngine::Log::get_logger()->trace(__VA_ARGS__)
#define SNAP_WARN(...)      SnapEngine::Log::get_logger()->warn(__VA_ARGS__)
#define SNAP_ERROR(...)     SnapEngine::Log::get_logger()->error(__VA_ARGS__)
#define SNAP_CRITICAL(...)  SnapEngine::Log::get_logger()->critical(__VA_ARGS__)