#include "VoxPch.h"
#include "Application.h"

#include "Vox/Core/Log.h"

#include "Vox/Renderer/Renderer.h"
#include "Vox/Scripting/ScriptEngine.h"

#include "Vox/Core/Input.h"
#include "Vox/Tools/PlatformTools.h"

namespace Vox
{
	Application* Application::m_Instance = nullptr; 
	
	Application::Application(const ApplicationSpec& spec) : m_Spec(spec)
	{
		VOX_PROFILE_FUNCTION();
		VOX_CORE_ASSERT(!m_Instance, "Application aleady exists!");
		m_Instance = this;

		if (!m_Spec.WorkingDirectory.empty())
		{
			std::filesystem::current_path(m_Spec.WorkingDirectory);
		}

		m_Window = Window::Create(WindowProps(m_Spec.Name));
		m_Window->SetEventCallback(VOX_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();
		ScriptEngine::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		VOX_PROFILE_FUNCTION();
		m_EventCallbacks.clear();
		ScriptEngine::Shutdown();
		Renderer::Shutdown();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		VOX_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(VOX_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(VOX_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			(*it)->OnEvent(e);
			if (e.Handled)
			{
				break;
			}
		}

		if (e.Handled)
			return;

		for (auto& eventCallback : m_EventCallbacks)
		{
			eventCallback(e);

			if (e.Handled)
				break;
		}
	}

	void Application::SubmitToMainThread(const std::function<void()>& function)
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);
		m_MainThreadQueue.emplace_back(function);
	}

	void Application::ProcessEvents()
	{
		std::scoped_lock<std::mutex> lock(m_EventQueueMutex);

		// Process custom event queue
		while (m_EventQueue.size() > 0)
		{
			auto& func = m_EventQueue.front();
			func();
			m_EventQueue.pop();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		VOX_PROFILE_FUNCTION();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		VOX_PROFILE_FUNCTION();
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Run()
	{
		VOX_PROFILE_FUNCTION();
		while (m_Running)
		{
			VOX_PROFILE_SCOPE("RunLoop");
			ProcessEvents();

			float time = Time::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			ExecuteMainThreadQueue();

			if (!m_Minimized)
			{
				{
					VOX_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{
					VOX_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		VOX_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

	void Application::ExecuteMainThreadQueue()
	{
		std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

		for (auto& func : m_MainThreadQueue)
		{
			func();
		}

		m_MainThreadQueue.clear();
	}
}

