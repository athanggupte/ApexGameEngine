#include "pch.h"
#include "TextureDescriptorTest.h"

#include "imgui.h"
#include "Apex/Graphics/Renderer/RenderCommands.h"
#include "Apex/Graphics/RenderPrimitives/Texture.h"
#include "Apex/Utils/Utils.h"

using namespace Apex;

namespace sandbox {

	namespace
	{
		Ref<Texture> s_Texture2D;
		Ref<Texture> s_Texture2D_1;
	}


	TextureDescriptorTest::TextureDescriptorTest()
		: Layer("Texture Descriptor Test")
	{
	}

	void TextureDescriptorTest::OnAttach()
	{
		{
			TextureDesc textureDesc;
			textureDesc.type = TextureType::Texture2D;
			textureDesc.name = "pusheen";
			textureDesc.srcPaths = { "assets/textures/pusheen-thug-life.png" };
			textureDesc.accessFormat = TextureAccessFormat::RGBA;
			textureDesc.internalFormat = TextureInternalFormat::RGBA8;
			textureDesc.dataType = TextureDataType::UBYTE;
			s_Texture2D = Texture::Create(textureDesc);
		}

		{
			TextureDesc textureDesc;
			textureDesc.type = TextureType::Texture2D;
			textureDesc.name = "pusheen-sRGB";
			textureDesc.srcPaths = { "assets/textures/pusheen-thug-life.png" };
			textureDesc.accessFormat = TextureAccessFormat::RGBA;
			textureDesc.internalFormat = TextureInternalFormat::SRGBA8;
			textureDesc.dataType = TextureDataType::UBYTE;
			s_Texture2D_1 = Texture::Create(textureDesc);
		}
	}

	void TextureDescriptorTest::OnDetach()
	{
		s_Texture2D.reset();
		s_Texture2D_1.reset();
	}

	void TextureDescriptorTest::OnUpdate(Timestep ts)
	{
		RenderCommands::Clear();
	}

	void TextureDescriptorTest::OnImGuiRender()
	{
		ImGui::Begin("Textures");
		ImGui::Text(TO_CSTRING(s_Texture2D->GetName()));
		ImGui::Image((void*)(intptr_t)s_Texture2D->GetID(), {256.f, 256.f}, {0, 1}, {1, 0});
		ImGui::Separator();
		ImGui::Text(TO_CSTRING(s_Texture2D_1->GetName()));
		ImGui::Image((void*)(intptr_t)s_Texture2D_1->GetID(), {256.f, 256.f}, {0, 1}, {1, 0});
		ImGui::End();
	}
}

