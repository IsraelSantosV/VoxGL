#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"
using namespace std;

namespace Vox
{
	class VOX_API Log
	{
	public:
		static void Init();

		inline static shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		inline static shared_ptr<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
		
	private:
		static shared_ptr<spdlog::logger> m_CoreLogger;
		static shared_ptr<spdlog::logger> m_ClientLogger;
	};
}

#define LOG_CORE_TRACE(...) ::Vox::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...) ::Vox::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...) ::Vox::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Vox::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_FATAL(...) ::Vox::Log::GetCoreLogger()->fatal(__VA_ARGS__)

#define LOG_TRACE(...) ::Vox::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) ::Vox::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) ::Vox::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::Vox::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...) ::Vox::Log::GetClientLogger()->fatal(__VA_ARGS__)

