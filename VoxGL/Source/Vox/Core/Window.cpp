#include "VoxPch.h"
#include "Vox/Core/Window.h"

#ifdef VOX_PLATFORM_WINDOWS
	#include "Platform/Windows/WinWindow.h"
#endif

namespace Vox
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
	#ifdef VOX_PLATFORM_WINDOWS
			return CreateScope<WinWindow>(props);
	#else
			VOX_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
	#endif
	}
}