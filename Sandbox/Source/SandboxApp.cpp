#include <Vox.h>
#include <Vox/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"

class Sandbox : public Vox::Application
{
public:
	Sandbox(const Vox::ApplicationSpec& spec) : Vox::Application(spec)
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
	ApplicationSpec spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Sandbox";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
