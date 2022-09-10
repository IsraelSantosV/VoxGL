#include "VoxPch.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <gl/GL.h>

namespace Vox
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{
		VOX_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		VOX_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		VOX_CORE_ASSERT(status, "Failed to initialize Glad!");

		LOG_CORE_INFO("OpenGL Info:");
		LOG_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		LOG_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		LOG_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		VOX_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), 
			"VoxGL requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		VOX_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}