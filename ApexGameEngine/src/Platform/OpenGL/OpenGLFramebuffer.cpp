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

		// Create Depth attachment
		if (m_DepthAttachment) {
			if (m_Specification.width > m_DepthAttachment->GetWidth() || m_Specification.height > m_DepthAttachment->GetHeight()) {
				m_DepthAttachment->Invalidate(m_Specification.width, m_Specification.height);
				glNamedFramebufferTexture(m_RendererID, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachment->GetID(), 0);
			}
		}
		
		// Create Renderbuffer if depth attachment not set
		if (!m_DepthAttachment && m_Specification.useDepth) {
			glCreateRenderbuffers(1, &m_RenderBuffer);
			glNamedRenderbufferStorage(m_RenderBuffer, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height);

			glNamedFramebufferRenderbuffer(m_RendererID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
		}

		// Invalidating Color attachments
		m_ActiveAttachments = std::max(m_ActiveAttachments, m_Specification.numColorAttachments);
		
		// Create color attachments owned by Framebuffer
		for (auto i = 0; i < m_Specification.numColorAttachments; i++) {
			m_ColorAttachments[i] = Texture2D::Create(m_Specification.width, m_Specification.height, SimpleTextureSpec);
			glNamedFramebufferTexture(m_RendererID, OpenGLColorAttachments[i], m_ColorAttachments[i]->GetID(), 0);
		}
		
		// Reset color attachments added externally
		for (auto i = m_Specification.numColorAttachments; i < m_ActiveAttachments; i++) {
			if (m_Specification.width > m_ColorAttachments[i]->GetWidth() || m_Specification.height > m_ColorAttachments[i]->GetHeight()) {
				m_ColorAttachments[i]->Invalidate(m_Specification.width, m_Specification.height);
				glNamedFramebufferTexture(m_RendererID, OpenGLColorAttachments[i], m_ColorAttachments[i]->GetID(), 0);
			}
		}
		
		// Specify Active attachments
		glNamedFramebufferDrawBuffers(m_ActiveAttachments, m_ActiveAttachments, OpenGLColorAttachments);
	}
	
	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.width = width;
		m_Specification.height = height;
		
		Invalidate();
	}
	
	void OpenGLFramebuffer::Bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.width, m_Specification.height);
		APEX_CORE_ASSERT(IsComplete(), "Framebuffer not complete!");
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
		APEX_CORE_ASSERT(texture->GetWidth() >= m_Specification.width && texture->GetHeight() >= m_Specification.height, "Insufficient size of attachment!");
		
		if (m_ActiveAttachments >= m_Specification.maxColorAttachments) {
			APEX_CORE_ERROR("Attempting to attach more than maximum attachments to Framebuffer!");
			return;
		}
		glNamedFramebufferTexture(m_RendererID, OpenGLColorAttachments[m_ActiveAttachments], texture->GetID(), 0);
		m_ColorAttachments[m_ActiveAttachments] = texture;
		m_ActiveAttachments++;
	}
	
	void OpenGLFramebuffer::AttachDepthTexture(const Ref<TextureDepth2D>& texture)
	{
		APEX_CORE_ASSERT(texture->GetWidth() >= m_Specification.width && texture->GetHeight() >= m_Specification.height, "Insufficient size of attachment!");

		glNamedFramebufferTexture(m_RendererID, GL_DEPTH_STENCIL_ATTACHMENT, texture->GetID(), 0);
		m_DepthAttachment = texture;
	}
	
	bool OpenGLFramebuffer::IsComplete() const
	{
		return glCheckNamedFramebufferStatus(m_RendererID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
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
