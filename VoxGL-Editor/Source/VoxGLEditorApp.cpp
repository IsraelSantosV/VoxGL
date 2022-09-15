#include <Vox.h>
#include <Vox/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Vox
{
	class VoxGLEditorApp : public Application
	{
	public:
		VoxGLEditorApp(const ApplicationSpec& spec) : Application(spec)
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication(AppCommandLineArgs args)
	{
		ApplicationSpec spec;
		spec.Name = "VoxGL Editor";
		spec.CommandLineArgs = args;

		return new VoxGLEditorApp(spec);
	}
}
