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

		std::shared_ptr<Shader> m_RGBShader;
		std::shared_ptr<VertexArray> m_TriangleVA;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;

	private:
		static Application* m_Instance;
	};

	Application* CreateApplication();
}


