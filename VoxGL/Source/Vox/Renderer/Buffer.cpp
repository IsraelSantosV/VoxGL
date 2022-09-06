#include "VoxPch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Vox
{
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: 
				VOX_CORE_ASSERT(false, "RendererApi::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		VOX_CORE_ASSERT(false, "Unknown RendererApi!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				VOX_CORE_ASSERT(false, "RendererApi::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return CreateRef<OpenGLIndexBuffer>(indices, size);
		}

		VOX_CORE_ASSERT(false, "Unknown RendererApi!");
		return nullptr;
	}
}