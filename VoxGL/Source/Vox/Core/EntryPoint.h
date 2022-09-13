#pragma once
#include "Vox/Core/Core.h"
#include "Vox/Core/Application.h"

#ifdef VOX_PLATFORM_WINDOWS

extern Vox::Application* Vox::CreateApplication(AppCommandLineArgs args);

int main(int argc, char** argv)
{
	Vox::Log::Init();

	VOX_PROFILE_BEGIN_SESSION("Startup", "VoxProfile-Startup.json");
	auto app = Vox::CreateApplication({ argc, argv });
	VOX_PROFILE_END_SESSION();

	VOX_PROFILE_BEGIN_SESSION("Runtime", "VoxProfile-Runtime.json");
	app->Run();
	VOX_PROFILE_END_SESSION();

	VOX_PROFILE_BEGIN_SESSION("Shutdown", "VoxProfile-Shutdown.json");
	delete app;
	VOX_PROFILE_END_SESSION();
}

#endif
