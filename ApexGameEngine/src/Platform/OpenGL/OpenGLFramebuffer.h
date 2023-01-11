#pragma once

#include "Apex/Graphics/RenderPrimitives/Framebuffer.h"

namespace Apex {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		explicit OpenGLFramebuffer(FramebufferSpec spec);
		~OpenGLFramebuffer() override;

		void Bind() const override;
		void Unbind() const override;

		void Invalidate() override;
		void Resize(uint32_t width, uint32_t height) override;

		void AddColorAttachment(TextureSpec textureSpec) override;
		void AttachDepthTexture(const Ref<Texture>& texture, bool stencil = false) override;

		void Blit(const Ref<Framebuffer>& targetFramebuffer, FramebufferTargetMask mask) override;

		[[nodiscard]] uint32_t GetColorAttachmentID(uint32_t index) const override { return m_ColorAttachments[index]->GetID(); }
		[[nodiscard]] const Ref<Texture2D>& GetColorAttachment(uint32_t index) const override { return m_ColorAttachments[index]; }
		[[nodiscard]] const Ref<Texture>& GetDepthAttachment() const override { return m_DepthAttachment; }

		[[nodiscard]] const FramebufferSpec& GetSpecification() const override { return m_Specification; }
		void SetSpecification(const FramebufferSpec& specification) override;

		[[nodiscard]] bool IsComplete() const;

	private:
		FramebufferSpec m_Specification;
		uint32_t m_RendererID = 0;
		uint32_t m_RenderBuffer = 0;
		Ref<Texture> m_DepthAttachment = nullptr;
		std::vector<Ref<Texture2D>> m_ColorAttachments;
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
