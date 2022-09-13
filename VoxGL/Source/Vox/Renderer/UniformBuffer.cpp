#include "VoxPch.h"
#include "Vox/Renderer/UniformBuffer.h"

#include "Vox/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformbuffer.h"

namespace Vox 
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:    
				VOX_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  
				return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		VOX_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
