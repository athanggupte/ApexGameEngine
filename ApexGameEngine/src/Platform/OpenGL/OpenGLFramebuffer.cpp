#include "apex_pch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

#include "Apex/Graphics/Renderer/RendererAPI.h"

namespace Apex {
	
	constexpr static GLenum OpenGLColorAttachments[] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7
	};

	static_assert(FramebufferSpec::MAX_COLOR_ATTACHMENTS <= sizeof(OpenGLColorAttachments), "Max color attachments exceeds OpenGL specifications!");

	
	static const char* GetFramebufferStatusString(GLenum status)
	{
		switch (status)
		{
		case GL_FRAMEBUFFER_COMPLETE: return "GL_FRAMEBUFFER_COMPLETE";
		case GL_FRAMEBUFFER_UNDEFINED: return "GL_FRAMEBUFFER_UNDEFINED";
		case GL_FRAMEBUFFER_UNSUPPORTED: return "GL_FRAMEBUFFER_UNSUPPORTED";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
		default: return "UNKNOWN";
		}
	}

#include "OpenGLHelpers.h"

	//////////////////////////////////////////////////////////////////////
	/*--------------------------Frame Buffer----------------------------*/
	//////////////////////////////////////////////////////////////////////

	OpenGLFramebuffer::OpenGLFramebuffer(FramebufferSpec spec)
		: m_Specification(spec), m_ColorAttachments(spec.MAX_COLOR_ATTACHMENTS, nullptr)
	{
		if (m_Specification.numColorAttachments || m_Specification.useDepth)
			OpenGLFramebuffer::Invalidate();
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
		if (m_Specification.useDepthBuffer || m_DepthAttachment) {
			if (m_DepthAttachment) {
				//if (m_Specification.width > m_DepthAttachment->GetWidth() || m_Specification.height > m_DepthAttachment->GetHeight()) {
					m_DepthAttachment->Invalidate(m_Specification.width, m_Specification.height);
				//}
			} else {
				m_DepthAttachment = Texture2D::Create(m_Specification.width, m_Specification.height, TextureSpec{
					                                      TextureAccessFormat::DEPTH, TextureInternalFormat::DEPTH24,
					                                      TextureDataType::FLOAT
				                                      });
			}
			glNamedFramebufferTexture(m_RendererID, GL_DEPTH_ATTACHMENT, m_DepthAttachment->GetID(), 0);
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
		if (m_Specification.numColorAttachments > 0) {
			glNamedFramebufferDrawBuffers(m_RendererID, m_Specification.numColorAttachments, OpenGLColorAttachments);
			glNamedFramebufferReadBuffer(m_RendererID, OpenGLColorAttachments[0]);
		} else {
			glNamedFramebufferDrawBuffer(m_RendererID, GL_NONE);
			glNamedFramebufferReadBuffer(m_RendererID, GL_NONE);
		}
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
		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		APEX_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, fmt::format("Framebuffer not complete! - {}", GetFramebufferStatusString(status)));
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

		auto idx = m_Specification.numColorAttachments++;

		if (m_Specification.multisample)
			m_ColorAttachments[idx] = Texture2DMS::Create(m_Specification.width, m_Specification.height, textureSpec, m_Specification.msSamples);
		else
			m_ColorAttachments[idx] = Texture2D::Create(m_Specification.width, m_Specification.height, textureSpec);

		if (!m_RendererID) {
			Invalidate();
		} else {
			glNamedFramebufferTexture(m_RendererID, OpenGLColorAttachments[idx], m_ColorAttachments[idx]->GetID(), 0);

			glNamedFramebufferDrawBuffers(m_RendererID, m_Specification.numColorAttachments, OpenGLColorAttachments);
			glNamedFramebufferReadBuffer(m_RendererID, OpenGLColorAttachments[0]);
		}
	}
	
	void OpenGLFramebuffer::AttachDepthTexture(const Ref<Texture>& texture, bool stencil)
	{
		APEX_CORE_ASSERT(texture->GetWidth() >= m_Specification.width && texture->GetHeight() >= m_Specification.height, "Insufficient size of attachment!");

		/*if (texture->GetType() == TextureType::Texture2DArray)
			glNamedFramebufferTextureLayer(m_RendererID, stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, texture->GetID(), 0, 0);
		else*/

		m_DepthAttachment = texture;

		if (!m_RendererID) {
			Invalidate();
		} else {
			glNamedFramebufferTexture(m_RendererID, stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, texture->GetID(), 0);
		}
	}

	void OpenGLFramebuffer::Blit(const Ref<Framebuffer>& targetFramebuffer, FramebufferTargetMask mask)
	{
		auto target = std::static_pointer_cast<OpenGLFramebuffer>(targetFramebuffer);
		glBlitNamedFramebuffer(m_RendererID, target->m_RendererID,
			0, 0, m_Specification.width, m_Specification.height,
			0, 0, target->m_Specification.width, target->m_Specification.height,
			GetGLFramebufferTargetMask(mask), (mask & FramebufferTargetBit::DEPTH || mask & FramebufferTargetBit::STENCIL) ? GL_NEAREST : GL_LINEAR);
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
