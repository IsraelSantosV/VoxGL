#pragma once

#include <memory>

#ifdef _WIN32
	#ifdef _WIN64
		#define VOX_PLATFORM_WINDOWS
	#else
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>

	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define VOX_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define VOX_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
#elif defined(__ANDROID__)
	#define VOX_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define VOX_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	#error "Unknown platform!"
#endif

#ifdef VOX_DEBUG
	#define VOX_ENABLE_ASSERTS
#endif

#ifdef VOX_ENABLE_ASSERTS
	#define VOX_ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
	#define VOX_CORE_ASSERT(x, ...) {if(!(x)) { LOG_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); }}
#else
	#define VOX_ASSERT(x, ...)
	#define VOX_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define VOX_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Vox
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}