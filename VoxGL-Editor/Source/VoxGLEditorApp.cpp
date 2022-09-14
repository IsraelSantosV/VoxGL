#include <Vox.h>
#include <Vox/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Vox
{
	class VoxGLEditorApp : public Application
	{
	public:
		VoxGLEditorApp(AppCommandLineArgs args) : Application("VoxGL-Editor", args)
		{
			PushLayer(new EditorLayer());
		}

		~VoxGLEditorApp()
		{
		}
	};

	Application* CreateApplication(AppCommandLineArgs args)
	{
		return new VoxGLEditorApp(args);
	}
}
