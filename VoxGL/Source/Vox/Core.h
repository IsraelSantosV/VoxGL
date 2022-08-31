#pragma once

#ifdef VOX_PLATFORM_WINDOWS
	#ifdef VOX_BUILD_DLL
		#define VOX_API __declspec(dllexport)
	#else
		#define VOX_API __declspec(dllimport)
	#endif
#else
	#error VoxGL only supports Windows!
#endif