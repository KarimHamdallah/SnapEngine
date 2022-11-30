#include "SnapPCH.h"
#include "Log.h"

std::shared_ptr<spdlog::logger> SnapEngine::Log::m_logger;

namespace SnapEngine
{

	void Log::init()
	{
		//spdlog::set_pattern("%^[%T] %n:  %v%$");
		spdlog::set_pattern("[%n]:  %v%$");

		m_logger = spdlog::stdout_color_mt("Snap Logger");
		m_logger->set_level(spdlog::level::trace);
	}

	void Log::free_memory()
	{
		// free memory
		m_logger.reset();
	}
}