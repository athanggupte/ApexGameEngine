#pragma once

#include "Texture.h"

namespace Apex {
	
	struct FramebufferSpec
	{
		uint32_t width, height;
		uint8_t numColorAttachments = 1;
		bool useDepth = true; // if numColorAttachments is 0 then useDepth is automatically set to true
		bool multisample = false;
		bool useHDR = false;
		uint32_t msSamples = 4;

		constexpr static uint8_t MAX_COLOR_ATTACHMENTS = 8;
	};
	
	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Invalidate() = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		
		virtual void AddColorAttachment(TextureSpec textureSpec = SimpleTextureSpec) = 0;
		virtual void AttachDepthTexture(const Ref<TextureDepth2D>& texture) = 0;

		virtual void Blit(const Ref<Framebuffer>& targetFramebuffer) = 0;

		[[nodiscard]] virtual uint32_t GetColorAttachmentID(uint32_t index = 0) const = 0;
		[[nodiscard]] virtual const Ref<Texture2D>& GetColorAttachment(uint32_t index = 0) const = 0;
		[[nodiscard]] virtual const Ref<TextureDepth2D>& GetDepthAttachment() const = 0;

		[[nodiscard]] virtual const FramebufferSpec& GetSpecification() const = 0;
		virtual void SetSpecification(const FramebufferSpec& specification) = 0;

		static Ref<Framebuffer> Create(FramebufferSpec spec);
	};


#if USE_DEPTH_BUFFER
	class DepthBuffer
	{
	public:
		virtual ~DepthBuffer() = default;

		virtual void BindTextures() const = 0;
		virtual void Unbind() const = 0;

		virtual const Ref<TextureDepth2D>& GetDepthTexture() const = 0;

		static Ref<DepthBuffer> Create();
	};
#endif

}
