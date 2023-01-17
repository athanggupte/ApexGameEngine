#pragma once
#include "Apex/Core/Layers/Layer.h"

namespace sandbox {

	class TextureDescriptorTest : public Apex::Layer
	{
	public:
		TextureDescriptorTest();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate(Apex::Timestep) override;
		void OnImGuiRender() override;
	};

}
