#pragma once

#include "Apex/Layers/Layer.h"
#include "Apex/Events/MouseEvent.h"
#include "Apex/Events/KeyEvent.h"
#include "Apex/Events/ApplicationEvent.h"

namespace Apex {

	class APEX_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		float m_Time = 0.0f;
	};

}