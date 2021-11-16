#include "apex_pch.h"
#include "OpenGLRendererAPI.h"

#include "Apex/Graphics/RenderPrimitives/VertexArray.h"

#include <glad/glad.h>

namespace Apex {

	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_MULTISAMPLE);
		APEX_CORE_TRACE("RendererAPI (OpenGL) initialized successfully!");
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4 & color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void OpenGLRendererAPI::Draw(uint32_t vertexCount)
	{
		glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, vertexCount, 1, 0);
	}

	void OpenGLRendererAPI::Draw(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexArray->GetVertexBuffers().at(0)->GetCount());
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		auto count = indexCount ? indexCount : vertexArray->GetIndexBuffers().at(0)->GetCount();
		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
	
	void OpenGLRendererAPI::DrawInstanced(const Ref<VertexArray>& vertexArray, size_t count)
	{
		vertexArray->Bind();
		glDrawElementsInstanced(GL_TRIANGLES, vertexArray->GetIndexBuffers().at(0)->GetCount(), GL_UNSIGNED_INT, nullptr, count);
	}

	void OpenGLRendererAPI::SetDepthTest(bool value)
	{
		if (value)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::SetDepthWriteMode(bool value)
	{
		glDepthMask(value ? GL_TRUE : GL_FALSE);
	}
	
	void OpenGLRendererAPI::SetDepthTestFunction(DepthStencilMode criterion)
	{
		glDepthFunc((GLenum)criterion);
	}
	
	void OpenGLRendererAPI::SetStencilTest(bool value)
	{
		if (value)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}
	
	void OpenGLRendererAPI::SetStencilTestFunction(DepthStencilMode criterion, int8_t refValue, uint8_t mask)
	{
		glStencilFunc((GLenum)criterion, refValue, mask);
	}
	
	void OpenGLRendererAPI::SetStencilWriteMask(uint32_t mask)
	{
		glStencilMask(mask);
	}
	
	void OpenGLRendererAPI::SetStencilAction(DepthStencilAction onStencilFail, DepthStencilAction onDepthFail, DepthStencilAction onPass)
	{
		glStencilOp((GLenum)onStencilFail, (GLenum)onDepthFail, (GLenum)onPass);
	}
	
	void OpenGLRendererAPI::SetCulling(bool value)
	{
		if (value) {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
		}
		else {
			glDisable(GL_CULL_FACE);
		}
	}

	void OpenGLRendererAPI::SetBlendMode(BlendingMode sourceMode, BlendingMode destMode)
	{
		glBlendFunc((GLenum)sourceMode, (GLenum)destMode);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, size_t width, size_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::ImageAccessBarrier()
	{
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//APEX_CORE_WARN("[OpenGL] :: `glMemoryBarrier` called on GL_SHADER_IMAGE_ACCESS_BARRIER_BIT");
	}
	
}
