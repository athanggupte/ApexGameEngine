#pragma once

#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/Framebuffer.h"

namespace Apex {

	class Blur 
	{
	public:
		Blur();

		Ref<Texture> GaussianBlur(int amount, const Ref<Texture>& inputTexture);

	private:
		Ref<Shader> m_BlurShader;
		Ref<Texture2D> m_VerBlurTex;
		Ref<Texture2D> m_HorBlurTex;
		Ref<Framebuffer> m_VerBlurFBO;
		Ref<Framebuffer> m_HorBlurFBO;
	};

}
