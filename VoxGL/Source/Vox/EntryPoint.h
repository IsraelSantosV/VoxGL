#pragma once

#ifdef VOX_PLATFORM_WINDOWS

extern Vox::Application* Vox::CreateApplication();

int main(int argc, char** argv)
{
	Vox::Log::Init();
	auto app = Vox::CreateApplication();
	app->Run();
	delete app;
}

#endif