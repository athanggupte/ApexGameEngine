#pragma once

#include "Apex/RenderPrimitives/FrameBuffer.h"

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

}