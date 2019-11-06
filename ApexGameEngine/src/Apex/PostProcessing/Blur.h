#pragma once

#include "Apex/Renderer/Texture.h"
#include "Apex/Renderer/VertexArray.h"
#include "Apex/Renderer/Shader.h"
#include "Apex/Renderer/FrameBuffer.h"

namespace Apex {

	class Blur 
	{
	public:
		Blur();

		Ref<Texture> GaussianBlur(int amount, const Ref<Texture>& inputTexture, const Ref<VertexArray>& vao);

	private:
		Ref<Shader> m_BlurShader;
		Ref<Texture2D_HDR> m_VerBlurTex;
		Ref<Texture2D_HDR> m_HorBlurTex;
		Ref<FrameBuffer> m_VerBlurFBO;
		Ref<FrameBuffer> m_HorBlurFBO;
	};

}