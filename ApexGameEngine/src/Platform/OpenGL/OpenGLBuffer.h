#pragma once

#include "Apex/Renderer/Buffer.h"

namespace Apex {

	//////////////////////////////////////////////////////////////////////
	/*-------------------------Vertex Buffer----------------------------*/
	//////////////////////////////////////////////////////////////////////
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(float* vertices, size_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const;
		virtual void UnBind() const;
		
	private:
		uint32_t m_RendererID;
	};

	//////////////////////////////////////////////////////////////////////
	/*-------------------------Index Buffer----------------------------*/
	//////////////////////////////////////////////////////////////////////
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, size_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const;
		virtual void UnBind() const;

		inline virtual uint32_t GetCount() const { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};

}