#include "VoxPch.h"
#include "Vox/Renderer/GraphicsContext.h"

#include "Vox/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Vox {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    
				VOX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  
				return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		VOX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}