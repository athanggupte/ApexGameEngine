#include <apexed_pch.h>

#include "PythonGraph.h"

namespace Apex {

	EditorTools::Node* PythonGraph::CreateNode(uint32_t nodeType) {
		APEX_CORE_WARN(__FUNCTION__);
		switch (nodeType)
		{
		case Apex::RESULT:
			return new ResultNode();
		case Apex::ADD:
			return new AddNode();
		case Apex::SUB:
			return new SubNode();
		case Apex::MUL:
			return new MulNode();
		case Apex::DIV:
			return new ErrorNode();
		default:
			return new ErrorNode();
		}
	}

	std::vector<std::string> PythonGraph::nodeTypes = {
		"Result",
		"Add", "Sub", "Mul", "Div"
	};

	///////////////////////////////////////////////////////////// 
	//////////////////// Function Definitions ///////////////////
	/////////////////////////////////////////////////////////////

	std::string AddNode::definition = R"(
def add(A, B):
	return A + B
)";

	std::string SubNode::definition = R"(
def sub(A, B):
	return A - B
)";

	std::string MulNode::definition = R"(
def mul(A, B):
	return A * B
)";

}
