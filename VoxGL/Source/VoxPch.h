#pragma once

#include "Vox/Core/PlatformDetection.h"

#ifdef VOX_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Vox/Core/Core.h"

#include "Vox/Core/Log.h"
#include "Vox/Debug/Instrumentor.h"

#ifdef VOX_PLATFORM_WINDOWS
	#include <Windows.h>
#endif