#pragma once

#include "PythonNodes.h"

namespace Apex {

	enum PythonNodeType
	{
		RESULT = 0,
		ADD, SUB, MUL, DIV
	};

	struct PythonGraph
	{
		static EditorTools::Node* CreateNode(uint32_t noteType);
		static std::vector<std::string> nodeTypes;

	};

}