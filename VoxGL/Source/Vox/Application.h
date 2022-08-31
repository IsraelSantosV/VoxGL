#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Vox
{
	class VOX_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();
	};

	Application* CreateApplication();
}


