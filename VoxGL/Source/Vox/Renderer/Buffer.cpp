#include "VoxPch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Vox
{
	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetApi())
		{
			case RendererAPI::None: 
				VOX_CORE_ASSERT(false, "RendererApi::None is currently not supported!");
				return nullptr;
			case RendererAPI::OpenGL:
				return new OpenGLVertexBuffer(vertices, size);
		}

		VOX_CORE_ASSERT(false, "Unknown RendererApi!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t size)
	{
		switch (Renderer::GetApi())
		{
			case RendererAPI::None:
				VOX_CORE_ASSERT(false, "RendererApi::None is currently not supported!");
				return nullptr;
			case RendererAPI::OpenGL:
				return new OpenGLIndexBuffer(indices, size);
		}

		VOX_CORE_ASSERT(false, "Unknown RendererApi!");
		return nullptr;
	}
}