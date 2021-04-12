#include "apex_pch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Apex {
	
	constexpr static GLenum OpenGLColorAttachments[] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
	};

	//////////////////////////////////////////////////////////////////////
	/*--------------------------Frame Buffer----------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLFramebuffer::OpenGLFramebuffer(FramebufferSpec spec)
		: m_Specification(spec), m_ColorAttachments(spec.maxColorAttachments, nullptr)
	{
		APEX_CORE_ASSERT(m_Specification.maxColorAttachments <= sizeof(OpenGLColorAttachments), "Max color attachments exceeds OpenGL specifications!");
		
		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		if (m_RenderBuffer)
			glDeleteRenderbuffers(1, &m_RenderBuffer);
		
		// Delete textures ==> handled by destructor of Texture2D and TextureDepth2D
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (m_RendererID) glDeleteFramebuffers(1, &m_RendererID);
		if (m_RenderBuffer) glDeleteRenderbuffers(1, &m_RenderBuffer);
		
		// Create Framebuffer
		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		// Create Depth attachment
		if (m_DepthAttachment) {
			m_DepthAttachment->Resize(m_Specification.width, m_Specification.height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetID(), 0);
		}
		
		// Create Renderbuffer if depth attachment not set
		if (!m_DepthAttachment && m_Specification.useDepth) {
			glCreateRenderbuffers(1, &m_RenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		// Invalidating Color attachments
		m_ActiveAttachments = std::max(m_ActiveAttachments, m_Specification.numColorAttachments);
		
		// Create color attachments owned by Framebuffer
		for (auto i = 0; i < m_Specification.numColorAttachments; i++) {
			m_ColorAttachments[i] = Texture2D::Create(m_Specification.width, m_Specification.height, SimpleTextureSpec);
			glFramebufferTexture2D(GL_FRAMEBUFFER, OpenGLColorAttachments[i], GL_TEXTURE_2D, m_ColorAttachments[i]->GetID(), 0);
		}
		
		// Reset color attachments added externally
		for (auto i = m_Specification.numColorAttachments; i < m_ActiveAttachments; i++) {
			m_ColorAttachments[i]->Resize(m_Specification.width, m_Specification.height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, OpenGLColorAttachments[i], GL_TEXTURE_2D, m_ColorAttachments[i]->GetID(), 0);
		}
		
		// Specify Active attachments
		glDrawBuffers(m_ActiveAttachments, OpenGLColorAttachments);
		
		// Check for completeness
		//APEX_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
		
		// Bind default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.width = width;
		m_Specification.height = height;
		
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		
		if (m_RenderBuffer) {
			glDeleteRenderbuffers(1, &m_RenderBuffer);
			
			glCreateRenderbuffers(1, &m_RenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height);

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		
		
		for (auto i = 0; i < m_ActiveAttachments; i++) {
			m_ColorAttachments[i]->Resize(m_Specification.width, m_Specification.height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, OpenGLColorAttachments[i], GL_TEXTURE_2D, m_ColorAttachments[i]->GetID(), 0);
		}
		
		// Create Depth attachment
		if (m_DepthAttachment) {
				m_DepthAttachment->Resize(m_Specification.width, m_Specification.height);
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFramebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.width, m_Specification.height);
		APEX_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFramebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFramebuffer::AddColorAttachment(TextureSpec textureSpec) {}
	/*{
		if (m_ActiveAttachments >= m_Specification.maxColorAttachments) {
			APEX_CORE_ERROR("Attempting to attach more than maximum attachments to Framebuffer!");
			return;
		}
		auto texture = Texture2D::Create(m_Specification.width, m_Specification.height, textureSpec);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, OpenGLColorAttachments[m_ActiveAttachments], GL_TEXTURE_2D, texture->GetID(), 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_ColorAttachments[m_ActiveAttachments] = texture;
		m_ColorAttachments++;
	}*/
	
	void OpenGLFramebuffer::AttachColorTexture(const Ref<Texture2D>& texture)
	{
		APEX_CORE_ASSERT(texture->GetWidth() >= m_Specification.width && texture->GetHeight() >= m_Specification.height, "Insufficient size of color attachment!");
		
		if (m_ActiveAttachments >= m_Specification.maxColorAttachments) {
			APEX_CORE_ERROR("Attempting to attach more than maximum attachments to Framebuffer!");
			return;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, OpenGLColorAttachments[m_ActiveAttachments], GL_TEXTURE_2D, texture->GetID(), 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_ColorAttachments[m_ActiveAttachments] = texture;
		m_ActiveAttachments++;
	}
	
	void OpenGLFramebuffer::AttachDepthTexture(const Ref<TextureDepth2D>& texture)
	{
		m_DepthAttachment = texture;
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->GetID(), 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	bool OpenGLFramebuffer::IsComplete() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}


	//////////////////////////////////////////////////////////////////////
	/*--------------------------Depth Buffer----------------------------*/
	//////////////////////////////////////////////////////////////////////
#if USE_DEPTH_BUFFER
	OpenGLDepthBuffer::OpenGLDepthBuffer()
	{
		glCreateFramebuffers(1, &m_RendererID);
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
#endif

}
