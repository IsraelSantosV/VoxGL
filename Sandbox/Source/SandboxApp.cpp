#include <Vox.h>

class ExampleLayer : public Vox::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override
	{
		LOG_INFO("ExampleLayer::Update");
	}

	void OnEvent(Vox::Event& event) override
	{
		LOG_INFO("{0}", event);
	}
};

class Sandbox : public Vox::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Vox::ImGuiLayer());
	}

	~Sandbox()
	{
	}
};

Vox::Application* Vox::CreateApplication()
{
	return new Sandbox();
}