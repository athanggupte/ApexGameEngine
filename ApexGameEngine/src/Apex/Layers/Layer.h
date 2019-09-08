#pragma once

#include "Apex/Events/Event.h"

namespace Apex {

	class APEX_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() = 0; 
		virtual void OnDetach() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnImGuiRender() = 0;
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }

	private:
		std::string m_DebugName;
	};

}