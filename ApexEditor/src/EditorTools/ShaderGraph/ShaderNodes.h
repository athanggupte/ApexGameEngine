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

	struct TextureNode : public EditorTools::Node
	{
		TextureNode();

		std::string GetFunction() { return ""; }
	};

	struct BlurNode : public EditorTools::Node
	{
		BlurNode();

		std::string GetFunction() { return ""; }
	};

}