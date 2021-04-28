#pragma once

#include "Apex/Core/Events/Event.h"
#include "Apex/Core/Timer.h"

namespace Apex {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() = 0; 
		virtual void OnDetach() = 0;
		virtual void OnUpdate(Timestep) = 0;
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }

	private:
		std::string m_DebugName;
	};

}
