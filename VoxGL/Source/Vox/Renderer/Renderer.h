#pragma once

namespace Vox
{
	enum class RendererAPI
	{
		None = 0,
		OpenGL = 1
	};

	class Renderer
	{
	public:
		inline static RendererAPI GetApi() { return m_RendererAPI; }
	private:
		static RendererAPI m_RendererAPI;
	};
}