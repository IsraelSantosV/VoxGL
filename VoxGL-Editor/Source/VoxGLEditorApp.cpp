#include <Vox.h>
#include <Vox/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Vox
{
	class VoxGLEditorApp : public Application
	{
	public:
		VoxGLEditorApp() : Application("VoxGL-Editor")
		{
			PushLayer(new EditorLayer());
		}

		~VoxGLEditorApp()
		{
		}
	};

	Application* CreateApplication()
	{
		return new VoxGLEditorApp();
	}
}