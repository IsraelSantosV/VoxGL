#pragma once

#include "Vox/Core/Core.h"

namespace Vox
{
	struct FramebufferSpec
	{
		uint32_t Width, Height;
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual uint32_t GetColorAttachmentRendererId() const = 0;

		virtual const FramebufferSpec& GetSpec() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpec& spec);
	};
}