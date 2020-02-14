#pragma once

#include "Apex/Renderer/FrameBuffer.h"

namespace Apex {

	class OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer(bool depth);
		virtual ~OpenGLFrameBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AttachTexture(const Ref<Texture2D>& texture) override;
		virtual const std::vector<Ref<Texture>>& GetTextures() const override { return m_Textures; }

		virtual bool IsComplete() const;

	private:
		uint32_t m_RendererID;
		uint32_t m_RenderBuffer;
		std::vector<Ref<Texture>> m_Textures;
		bool m_HDR;
	};

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

}