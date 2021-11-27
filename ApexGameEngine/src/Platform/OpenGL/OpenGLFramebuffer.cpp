#include "apex_pch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Apex {
	
	constexpr static GLenum OpenGLColorAttachments[] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
	};

	static_assert(FramebufferSpec::MAX_COLOR_ATTACHMENTS <= sizeof(OpenGLColorAttachments), "Max color attachments exceeds OpenGL specifications!");

	//////////////////////////////////////////////////////////////////////
	/*--------------------------Frame Buffer----------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLFramebuffer::OpenGLFramebuffer(FramebufferSpec spec)
		: m_Specification(spec), m_ColorAttachments(spec.MAX_COLOR_ATTACHMENTS, nullptr)
	{
		if (m_Specification.numColorAttachments || m_Specification.useDepth)
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
			if (m_Specification.multisample)
				glNamedRenderbufferStorageMultisample(m_RenderBuffer, m_Specification.msSamples, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height);
			else
				glNamedRenderbufferStorage(m_RenderBuffer, GL_DEPTH24_STENCIL8, m_Specification.width, m_Specification.height);

			glNamedFramebufferRenderbuffer(m_RendererID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBuffer);
		}

		// Invalidating Color attachments
		for (auto i = 0; i < m_Specification.numColorAttachments; i++) {
			// If the attachment has NOT been initialized then create a new default texture, SimpleTextureSpec (+ multisample if reqd)
			if (m_ColorAttachments[i]) {
				m_ColorAttachments[i]->Invalidate(m_Specification.width, m_Specification.height);
			} else {
				if (m_Specification.multisample)
					m_ColorAttachments[i] = Texture2DMS::Create(m_Specification.width, m_Specification.height, SimpleTextureSpec, m_Specification.msSamples);
				else
					m_ColorAttachments[i] = Texture2D::Create(m_Specification.width, m_Specification.height, SimpleTextureSpec);
			}
			glNamedFramebufferTexture(m_RendererID, OpenGLColorAttachments[i], m_ColorAttachments[i]->GetID(), 0);
		}
		
		// Specify Active attachments
		glNamedFramebufferDrawBuffers(m_RendererID, m_Specification.numColorAttachments, OpenGLColorAttachments);
		glNamedFramebufferReadBuffer(m_RendererID, OpenGLColorAttachments[0]);
	}
	
	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specification.width = width;
		m_Specification.height = height;
		
		Invalidate();
	}
	
	void OpenGLFramebuffer::Bind() const
	{
		APEX_CORE_ASSERT(IsComplete(), "Framebuffer not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.width, m_Specification.height);
	}
	
	void OpenGLFramebuffer::Unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	void OpenGLFramebuffer::AddColorAttachment(TextureSpec textureSpec)
	{
		if (m_Specification.numColorAttachments >= m_Specification.MAX_COLOR_ATTACHMENTS) {
			APEX_CORE_ERROR("Attempting to attach more than maximum attachments to Framebuffer!");
			return;
		}

		if (m_Specification.multisample)
			m_ColorAttachments[m_Specification.numColorAttachments] = Texture2DMS::Create(m_Specification.width, m_Specification.height, textureSpec, m_Specification.msSamples);
		else
			m_ColorAttachments[m_Specification.numColorAttachments] = Texture2D::Create(m_Specification.width, m_Specification.height, textureSpec);
					
		m_Specification.numColorAttachments++;
		Invalidate();
	}
	
	void OpenGLFramebuffer::AttachDepthTexture(const Ref<TextureDepth2D>& texture)
	{
		APEX_CORE_ASSERT(texture->GetWidth() >= m_Specification.width && texture->GetHeight() >= m_Specification.height, "Insufficient size of attachment!");

		glNamedFramebufferTexture(m_RendererID, GL_DEPTH_STENCIL_ATTACHMENT, texture->GetID(), 0);
		m_DepthAttachment = texture;
	}

	void OpenGLFramebuffer::Blit(const Ref<Framebuffer>& targetFramebuffer)
	{
		auto target = std::static_pointer_cast<OpenGLFramebuffer>(targetFramebuffer);
		glBlitNamedFramebuffer(m_RendererID, target->m_RendererID,
			0, 0, m_Specification.width, m_Specification.height,
			0, 0, target->m_Specification.width, target->m_Specification.height,
			GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	void OpenGLFramebuffer::SetSpecification(const FramebufferSpec& specification)
	{
		m_Specification = specification;
		Invalidate();
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
