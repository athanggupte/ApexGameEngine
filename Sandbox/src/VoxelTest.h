#pragma once
#include "Apex/Core/Layers/Layer.h"

namespace sandbox {

	class VoxelTest : public Apex::Layer
	{
	public:
		VoxelTest();

		void OnAttach() override;
		void OnDetach() override {}
		void OnUpdate(Apex::Timestep) override;
	};

}
