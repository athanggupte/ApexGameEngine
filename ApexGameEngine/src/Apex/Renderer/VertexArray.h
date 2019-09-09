#pragma once

#include "Buffer.h"

namespace Apex {

	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer> vertexBuffer) = 0;
		virtual void AddIndexBuffer(const Ref<IndexBuffer> IndexBuffer) = 0;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		virtual const std::vector<Ref<IndexBuffer>>& GetIndexBuffers() const = 0;

		static Ref<VertexArray> Create();
	};

}