#include "VoxPch.h"
#include "Log.h"

namespace Vox
{
	shared_ptr<spdlog::logger> Log::m_CoreLogger;
	shared_ptr<spdlog::logger> Log::m_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		m_CoreLogger = spdlog::stdout_color_mt("Vox");
		m_CoreLogger->set_level(spdlog::level::trace);

		m_ClientLogger = spdlog::stdout_color_mt("App");
		m_CoreLogger->set_level(spdlog::level::trace);
	}
}