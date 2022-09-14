#include <Vox.h>
#include <Vox/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"

class Sandbox : public Vox::Application
{
public:
	Sandbox(Vox::AppCommandLineArgs args)
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}
};

Vox::Application* Vox::CreateApplication(Vox::AppCommandLineArgs args)
{
	return new Sandbox(args);
}
