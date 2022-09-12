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
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual uint32_t GetColorAttachmentRendererId(uint32_t index = 0) const override 
		{ 
			VOX_CORE_ASSERT(index < m_ColorAttachments.size());
			return m_ColorAttachments[index];
		};

		virtual const FramebufferSpec& GetSpec() const { return m_Spec; }
	private:
		uint32_t m_RendererId = 0;
		FramebufferSpec m_Spec;

		std::vector<FramebufferTextureSpec> m_ColorAttachmentSpecs;
		FramebufferTextureSpec m_DepthAttachmentSpec = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};
}
