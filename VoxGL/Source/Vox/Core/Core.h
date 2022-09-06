#pragma once

#include <memory>

#ifdef VOX_PLATFORM_WINDOWS
#if VOX_DYNAMIC_LINK
	#ifdef VOX_BUILD_DLL
		#define VOX_API __declspec(dllexport)
	#else
		#define VOX_API __declspec(dllimport)
	#endif
#else
	#define VOX_API
#endif
#else
	#error VoxGL only supports Windows!
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

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}