#pragma once

#include "glm/glm.hpp"
#include "Apex/Renderer/VertexArray.h"

namespace Apex {

	class Mesh
	{
	public:
		Mesh(float* vertices, size_t vCount, uint32_t* indices, size_t iCount);
		virtual ~Mesh() = default;

		virtual inline void Bind() { m_VAO->Bind(); }
		virtual inline const Ref<VertexArray> GetVAO() { return m_VAO; }

	private:
		Ref<VertexArray> m_VAO;
	};

}