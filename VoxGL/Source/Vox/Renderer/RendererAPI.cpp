#include "VoxPch.h"
#include "RendererApi.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Vox
{
	RendererAPI::API RendererAPI::m_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (m_API)
		{
			case RendererAPI::API::None:    
				VOX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  
				return CreateScope<OpenGLRendererAPI>();
		}

		VOX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}