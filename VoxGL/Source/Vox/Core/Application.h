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
	struct AppCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			VOX_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpec
	{
		std::string Name = "VoxGL Application";
		std::string WorkingDirectory;
		AppCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		Application(const ApplicationSpec& spec);
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *m_Instance; }
		inline Window& GetWindow() { return *m_Window; }

		const ApplicationSpec& GetSpec() const { return m_Spec; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		ApplicationSpec m_Spec;

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* m_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(AppCommandLineArgs args);
}


