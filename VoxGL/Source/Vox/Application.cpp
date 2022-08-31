#include "VoxPch.h"
#include "Application.h"

#include "Vox/Events/AplicationEvent.h"
#include "Vox/Log.h"

namespace Vox
{
	Application::Application()
	{
	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			LOG_TRACE(e);
		}

		while (true);
	}
}

