#pragma once

#include "Vox/Renderer/Framebuffer.h"

namespace Vox
{
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpec& spec);
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual uint32_t GetColorAttachmentRendererId() const override { return m_ColorAttachment; };

		virtual const FramebufferSpec& GetSpec() const { return m_Spec; }
	private:
		uint32_t m_RendererId = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		FramebufferSpec m_Spec;
	};
}