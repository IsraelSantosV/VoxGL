#pragma once

#include "Core.h"
#include "Window.h"

#include "Vox/LayerStack.h"
#include "Events/Event.h"
#include "Vox/Events/AplicationEvent.h"

#include "Vox/ImGui/ImGuiLayer.h"

#include "Vox/Renderer/Shader.h"
#include "Vox/Renderer/Buffer.h"
#include "Vox/Renderer/VertexArray.h"

#include "Vox/Core/Timestep.h"

#include "Vox/Renderer/OrthographicCamera.h"

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

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* m_Instance;
	};

	Application* CreateApplication();
}


