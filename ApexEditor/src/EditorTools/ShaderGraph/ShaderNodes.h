#pragma once

#include "../NodeGraph/NodeGraph.h"

namespace Apex::EditorTools {

	struct OutputNode : public EditorTools::Node
	{
		static std::string definition;

		OutputNode();

		std::string GetFunction()
		{
			return " = Res#";
		}
	};

}