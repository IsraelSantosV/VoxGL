#pragma once

#include "Core.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Vox
{
	class VOX_API Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
		
	private:
		static std::shared_ptr<spdlog::logger> m_CoreLogger;
		static std::shared_ptr<spdlog::logger> m_ClientLogger;
	};
}

#define LOG_CORE_TRACE(...) ::Vox::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...) ::Vox::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...) ::Vox::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Vox::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...) ::Vox::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define LOG_TRACE(...) ::Vox::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::Vox::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::Vox::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Vox::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...) ::Vox::Log::GetClientLogger()->critical(__VA_ARGS__)

