#pragma once

#include "Apex/Core/Layers/Layer.h"
#include "Apex/Core/Events/MouseEvent.h"
#include "Apex/Core/Events/KeyEvent.h"
#include "Apex/Core/Events/ApplicationEvent.h"

namespace Apex {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep) override {}
		virtual void OnImGuiRender() override {}
		virtual void OnEvent(Event& event) override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
		bool m_BlockEvents = true;
	};

}
