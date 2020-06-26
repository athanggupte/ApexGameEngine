#pragma once

#include "../NodeGraph/NodeGraph.h"

namespace Apex {

	// ErrorNode -> DEBUG purposes
	struct ErrorNode : public EditorTools::Node
	{
		static std::string definition;

		ErrorNode()
		{
			name = "Error";
			inputSlots = {};
			outputSlots = {};
		}

		std::string GetFunction()
		{
			return "";
		}
	};

	// Final in an algorithm -> [In: 1], [Out: 0]
	struct ResultNode : public EditorTools::Node
	{
		static std::string definition;

		ResultNode()
		{
			name = "Result";
			inputSlots = {
				new EditorTools::Slot({ "Res" })
			};
			outputSlots = {};
			size = { 6.f, 0.f };
		}

		std::string GetFunction()
		{
			return "__result__ = Res#";
		}
	};

	struct AddNode : public EditorTools::Node
	{
		static std::string definition;

		AddNode()
		{
			name = "Add";
			inputSlots = {
				new EditorTools::Slot({ "A" }), new EditorTools::Slot({ "B" })
			};
			outputSlots = {
				new EditorTools::Slot({ "C" })
			};
			size = { 8.f, 0.f };
		}

		std::string GetFunction() override
		{
			return "C# = add(A#, B#)";
		}
	};

	struct SubNode : public EditorTools::Node
	{
		static std::string definition;

		SubNode()
		{
			name = "Sub";
			inputSlots = {
				new EditorTools::Slot({ "A" }), new EditorTools::Slot({ "B" })
			};
			outputSlots = {
				new EditorTools::Slot({ "C" })
			};
			size = { 8.f, 0.f };
		}

		std::string GetFunction() override
		{
			return "C# = sub(A#, B#)";
		}
	};

	struct MulNode : public EditorTools::Node
	{
		static std::string definition;

		MulNode()
		{
			name = "Mul";
			inputSlots = {
				new EditorTools::Slot({ "A" }), new EditorTools::Slot({ "B" })
			};
			outputSlots = {
				new EditorTools::Slot({ "C" })
			};
			size = { 8.f, 0.f };
		}

		std::string GetFunction() override
		{
			return "C# = mul(A#, B#)";
		}
	};

}