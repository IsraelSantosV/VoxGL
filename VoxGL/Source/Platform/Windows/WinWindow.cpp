#include "VoxPch.h"
#include "WinWindow.h"

#include "Vox/Core/Input.h"

#include "Vox/Events/AplicationEvent.h"
#include "Vox/Events/MouseEvent.h"
#include "Vox/Events/KeyEvent.h"

#include "Vox/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLContext.h"

namespace Vox
{
	float Window::m_HighDpiScaleFactor = 1.0f;
	static uint8_t m_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		LOG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	WinWindow::WinWindow(const WindowProps& props)
	{
		VOX_PROFILE_FUNCTION();

		Init(props);
	}

	WinWindow::~WinWindow() 
	{
		VOX_PROFILE_FUNCTION();

		Shutdown();
	}

	void WinWindow::Init(const WindowProps& props)
	{
		VOX_PROFILE_FUNCTION();

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		LOG_CORE_INFO("Creating window {0} ({1}, {2})",
			props.Title, props.Width, props.Height);

		if (m_GLFWWindowCount == 0)
		{
			VOX_PROFILE_SCOPE("GLFWInit");

			int success = glfwInit();
			VOX_CORE_ASSERT(success, "Could not initialize GLFW!");

			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			VOX_PROFILE_SCOPE("GLFWCreateWindow");

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			float xScale, yScale;
			glfwGetMonitorContentScale(monitor, &xScale, &yScale);

			if (xScale > 1.0f || yScale > 1.0f)
			{
				m_HighDpiScaleFactor = yScale;
			}

			glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

#if defined(VOX_DEBUG)
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height,
				m_Data.Title.c_str(), nullptr, nullptr);
			++m_GLFWWindowCount;
		}

		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		//GLFW Callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int character)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(static_cast<KeyCode>(character));
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WinWindow::Shutdown() 
	{
		VOX_PROFILE_FUNCTION();

		glfwDestroyWindow(m_Window);
		--m_GLFWWindowCount;

		if (m_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WinWindow::OnUpdate()
	{
		VOX_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WinWindow::SetVSync(bool enabled)
	{
		VOX_PROFILE_FUNCTION();

		if (enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}

		m_Data.VSync = enabled;
	}

	bool WinWindow::IsVSync() const
	{
		return m_Data.VSync;
	}


}
