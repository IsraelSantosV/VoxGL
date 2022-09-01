#pragma once

#include "Core.h"
#include "Window.h"

#include "Vox/LayerStack.h"
#include "Events/Event.h"
#include "Vox/Events/AplicationEvent.h"

#include "Vox/ImGui/ImGuiLayer.h"

namespace Vox
{
	class VOX_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline static Application& Get() { return *m_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* m_Instance;
	};

	Application* CreateApplication();
}


