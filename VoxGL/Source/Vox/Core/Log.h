#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include "Vox/Core/Core.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Vox
{
	class Log
	{
	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return m_ClientLogger; }
		
	private:
		static Ref<spdlog::logger> m_CoreLogger;
		static Ref<spdlog::logger> m_ClientLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternio)
{
	return os << glm::to_string(quaternio);
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

