#pragma once

#include "Apex/Graphics/RenderPrimitives/Framebuffer.h"

namespace Apex {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(FramebufferSpec spec);
		virtual ~OpenGLFramebuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Invalidate() override;
		virtual void Resize(uint32_t width, uint32_t height) override;
		
		virtual void AddColorAttachment(TextureSpec textureSpec) override;
		virtual void AttachColorTexture(const Ref<Texture2D>& texture) override;
		virtual void AttachDepthTexture(const Ref<TextureDepth2D>& texture) override;
		
		virtual uint32_t GetColorAttachmentID(uint32_t index) const override { return m_ColorAttachments[index]->GetID(); }
		virtual const Ref<Texture2D>& GetColorAttachment(uint32_t index) const override { return m_ColorAttachments[index]; }
		virtual const Ref<TextureDepth2D>& GetDepthAttachment() const override { return m_DepthAttachment; }

		virtual const FramebufferSpec& GetSpecification() const override { return m_Specification; }
		
		virtual bool IsComplete() const;

	private:
		FramebufferSpec m_Specification;
		uint32_t m_RendererID = 0;
		uint32_t m_RenderBuffer = 0;
		Ref<TextureDepth2D> m_DepthAttachment = nullptr;
		std::vector<Ref<Texture2D>> m_ColorAttachments;
		uint8_t m_ActiveAttachments = 0;
	};

#if USE_DEPTH_BUFFER
	class OpenGLDepthBuffer : public DepthBuffer
	{
	public:
		OpenGLDepthBuffer();
		virtual ~OpenGLDepthBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual const Ref<TextureDepth2D>& GetDepthTexture() const override { return m_DepthTexture; }

	private:
		uint32_t m_RendererID;
		Ref<TextureDepth2D> m_DepthTexture;
	};
#endif

}
