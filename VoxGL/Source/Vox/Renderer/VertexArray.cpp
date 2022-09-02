#include "VoxPch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Vox
{
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
				VOX_CORE_ASSERT(false, "RendererApi::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL:
				return new OpenGLVertexArray();
		}

		VOX_CORE_ASSERT(false, "Unknown RendererApi!");
		return nullptr;
	}
}