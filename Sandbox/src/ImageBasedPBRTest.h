#pragma once
#include "Apex/Core/Layers/Layer.h"

namespace sandbox {

	class ImageBasedPBRTest : public Apex::Layer
	{
	public:
		ImageBasedPBRTest();

		void OnAttach() override;
		void OnDetach() override {}
		void OnUpdate(Apex::Timestep) override;
		void OnEvent(Apex::Event& event) override;
		void OnImGuiRender() override;
	};

}
