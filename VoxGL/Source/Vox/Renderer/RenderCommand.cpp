#include "VoxPch.h"
#include "RenderCommand.h"

namespace Vox
{
	Scope<RendererAPI> RenderCommand::m_RendererAPI = RendererAPI::Create();
}