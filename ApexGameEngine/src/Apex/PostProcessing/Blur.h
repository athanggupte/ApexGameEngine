#pragma once

#include "Apex/RenderPrimitives/Texture.h"
#include "Apex/RenderPrimitives/VertexArray.h"
#include "Apex/RenderPrimitives/Shader.h"
#include "Apex/RenderPrimitives/FrameBuffer.h"

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