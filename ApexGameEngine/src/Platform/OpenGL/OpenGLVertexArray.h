#pragma once

#include "Apex/Renderer/VertexArray.h"

namespace Apex {

	class APEX_API OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer> vertexBuffer) override;
		virtual void AddIndexBuffer(const std::shared_ptr<IndexBuffer> indexBuffer) override;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		virtual const std::vector<std::shared_ptr<IndexBuffer>>& GetIndexBuffers() const override { return m_IndexBuffers; }

	private:
		uint32_t m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::vector<std::shared_ptr<IndexBuffer>> m_IndexBuffers;

	};

}