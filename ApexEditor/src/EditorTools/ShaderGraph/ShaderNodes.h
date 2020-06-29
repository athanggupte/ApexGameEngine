#pragma once

#include "Node.h"

namespace Apex::EditorTools {

	struct OutputNode : public EditorTools::Node
	{
		//static std::string definition;

		OutputNode();

		std::string GetFunction();
	};

	struct TestNode : public EditorTools::Node
	{
		//static std::string definition;

		TestNode();

		std::string GetFunction();
	};

}