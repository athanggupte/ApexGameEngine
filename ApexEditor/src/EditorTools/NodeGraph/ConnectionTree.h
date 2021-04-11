#pragma once

#include "NodeGraph.h"

namespace Apex::EditorTools {

	struct ConnectionNode
	{
		Connection* data;
		ConnectionNode* parent;
		std::vector<ConnectionNode*> children;

		//inline bool IsNull() { return (data == nullptr); }
	};

	class ConnectionTree
	{
	public:

		void Add(Connection& connection);

	private:
		std::list<ConnectionNode> m_ConnectionTrees;
	};

}