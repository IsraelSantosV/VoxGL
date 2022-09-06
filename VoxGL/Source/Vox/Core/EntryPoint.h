#pragma once
#include "Vox/Core/Core.h"

#ifdef VOX_PLATFORM_WINDOWS

extern Vox::Application* Vox::CreateApplication();

int main(int argc, char** argv)
{
	Vox::Log::Init();

	VOX_PROFILE_BEGIN_SESSION("Startup", "VoxProfile-Startup.json");
	auto app = Vox::CreateApplication();
	VOX_PROFILE_END_SESSION();

	VOX_PROFILE_BEGIN_SESSION("Runtime", "VoxProfile-Runtime.json");
	app->Run();
	VOX_PROFILE_END_SESSION();

	VOX_PROFILE_BEGIN_SESSION("Startup", "VoxProfile-Shutdown.json");
	delete app;
	VOX_PROFILE_END_SESSION();
}

#endif