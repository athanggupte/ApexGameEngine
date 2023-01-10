#pragma once
#include "Apex/Core/Layers/Layer.h"

namespace Apex
{
	class BloomPass;
	class Texture2D;
}

namespace sandbox
{
	class BloomTest : public Apex::Layer
	{
	public:
		BloomTest();

		void OnAttach() override;
		void OnDetach() override {}
		void OnUpdate(Apex::Timestep) override;
		void OnImGuiRender() override;

	private:
		Apex::Ref<Apex::Texture2D> m_InputTexture;
		Apex::Ref<Apex::Texture2D> m_OutputTexture;
	};

}
