#include <apexed_pch.h>
#include "ConnectionTree.h"

namespace Apex::EditorTools {

	void ConnectionTree::Add(Connection& connection)
	{

		if (connection.outputSlot->parentNode->outputSlots.empty()) {
			// Make ROOT
			return;
		}

		// else 
		// search trees for 

		m_ConnectionTrees.push_back({ &connection });


	}

}
