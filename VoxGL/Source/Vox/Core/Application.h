#pragma once

#include "Vox/Core/Core.h"
#include "Vox/Core/Window.h"

#include "Vox/Core/LayerStack.h"
#include "Vox/Events/Event.h"
#include "Vox/Events/AplicationEvent.h"

#include "Vox/Core/Timestep.h"

#include "Vox/ImGui/ImGuiLayer.h"
#include <queue>

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
		using EventCallbackFn = std::function<void(Event&)>;

	public:
		Application(const ApplicationSpec& spec);
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void AddEventCallback(const EventCallbackFn& eventCallback) { m_EventCallbacks.push_back(eventCallback); }

		template<typename Func>
		void QueueEvent(Func&& func)
		{
			m_EventQueue.push(func);
		}

		/// Creates & Dispatches an event either immediately, or adds it to an event queue which will be proccessed at the end of each frame
		template<typename TEvent, bool DispatchImmediately = false, typename... TEventArgs>
		void DispatchEvent(TEventArgs&&... args)
		{
			static_assert(std::is_assignable_v<Event, TEvent>);

			std::shared_ptr<TEvent> event = std::make_shared<TEvent>(std::forward<TEventArgs>(args)...);
			if constexpr (DispatchImmediately)
			{
				OnEvent(*event);
			}
			else
			{
				std::scoped_lock<std::mutex> lock(m_EventQueueMutex);
				m_EventQueue.push([event]() { Application::Get().OnEvent(*event); });
			}
		}

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		inline static Application& Get() { return *m_Instance; }
		inline Window& GetWindow() { return *m_Window; }

		const ApplicationSpec& GetSpec() const { return m_Spec; }
		void SubmitToMainThread(const std::function<void()>& function);
	private:
		void ProcessEvents();
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void ExecuteMainThreadQueue();
	private:
		ApplicationSpec m_Spec;

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;

		std::mutex m_EventQueueMutex;
		std::queue<std::function<void()>> m_EventQueue;
		std::vector<EventCallbackFn> m_EventCallbacks;
	private:
		static Application* m_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(AppCommandLineArgs args);
}


