#pragma once

#include "Apex/Graphics/RenderPrimitives/VertexArray.h"

namespace Apex {

	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer> vertexBuffer) override;
		virtual void AddIndexBuffer(const Ref<IndexBuffer> indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		virtual const std::vector<Ref<IndexBuffer>>& GetIndexBuffers() const override { return m_IndexBuffers; }

	private:
		uint32_t m_RendererID;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		std::vector<Ref<IndexBuffer>> m_IndexBuffers;

	};

}