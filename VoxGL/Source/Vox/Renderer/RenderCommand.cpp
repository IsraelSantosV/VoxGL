#include "VoxPch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Vox
{
	RendererAPI* RenderCommand::m_RendererAPI = new OpenGLRendererAPI;
}