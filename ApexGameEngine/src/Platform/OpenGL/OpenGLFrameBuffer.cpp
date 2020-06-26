#include "apex_pch.h"
#include "OpenGLFrameBuffer.h"

#include <glad/glad.h>

namespace Apex {

	static GLenum GetColorAttachment(size_t id)
	{
		switch (id)
		{
		case 0: return GL_COLOR_ATTACHMENT0;
		case 1: return GL_COLOR_ATTACHMENT1;
		case 2: return GL_COLOR_ATTACHMENT2;
		case 3: return GL_COLOR_ATTACHMENT3;
		case 4: return GL_COLOR_ATTACHMENT4;
		case 5: return GL_COLOR_ATTACHMENT5;
		default: return GL_COLOR_ATTACHMENT0;
		}
	}


	//////////////////////////////////////////////////////////////////////
	/*--------------------------Frame Buffer----------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLFrameBuffer::OpenGLFrameBuffer(bool depth)
		: m_HDR(false)
	{
		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		if (depth) {
			glGenRenderbuffers(1, &m_RenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		GLenum attachments[6] = {
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5
		};

		glDrawBuffers(6, attachments);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void OpenGLFrameBuffer::Bind() const
	{
		if (!IsComplete()) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			APEX_CORE_ERROR("Framebuffer not complete");
		}
	}
	
	void OpenGLFrameBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFrameBuffer::AttachTexture(const Ref<Texture2D>& texture)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GetColorAttachment(m_Textures.size()), GL_TEXTURE_2D, texture->GetID(), 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_Textures.push_back(texture);
	}

	bool OpenGLFrameBuffer::IsComplete() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}


	//////////////////////////////////////////////////////////////////////
	/*--------------------------Depth Buffer----------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLDepthBuffer::OpenGLDepthBuffer()
	{
		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		m_DepthTexture = TextureDepth2D::Create();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture->GetID(), 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	OpenGLDepthBuffer::~OpenGLDepthBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
	}

	void OpenGLDepthBuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void OpenGLDepthBuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}