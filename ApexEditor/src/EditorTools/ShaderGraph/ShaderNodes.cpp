#include <apex_pch.h>
#include "ShaderNodes.h"

namespace Apex::EditorTools {

	OutputNode::OutputNode()
	{
		name = "Output";
		inputSlots = {
			new EditorTools::Slot({ "Color" })
		};
		outputSlots = {};
		size = { 6.f, 0.f };
	}

}