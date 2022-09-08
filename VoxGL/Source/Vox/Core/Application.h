#pragma once

#include "Vox/Core/Core.h"
#include "Vox/Core/Window.h"

#include "Vox/Core/LayerStack.h"
#include "Vox/Events/Event.h"
#include "Vox/Events/AplicationEvent.h"

#include "Vox/Core/Timestep.h"

#include "Vox/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Vox
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void Close();

		inline static Application& Get() { return *m_Instance; }
		inline Window& GetWindow() { return *m_Window; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* m_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();
}


