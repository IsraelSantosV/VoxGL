#include "VoxPch.h"
#include "Vox/Core/Input.h"

#ifdef VOX_PLATFORM_WINDOWS
	#include "Platform/Windows/WinInput.h"
#endif

namespace Vox 
{
	Scope<Input> Input::m_Instance = Input::Create();

	Scope<Input> Input::Create()
	{
	#ifdef VOX_PLATFORM_WINDOWS
			return CreateScope<WinInput>();
	#else
			VOX_CORE_ASSERT(false, "Unknown platform!");
			return nullptr;
	#endif
	}
}