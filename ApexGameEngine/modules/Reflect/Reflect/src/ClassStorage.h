#pragma once

#include "Field.h"
#include "Function.h"

#include <array>

namespace detail {

	template <typename Type, size_t NumBases, size_t NumFields, size_t NumFuncs, size_t NumTemplateArgs = 0>
	struct ClassStorage
	{
		constexpr static size_t s_NumBases = NumBases;
		constexpr static size_t s_NumFields = NumFields;
		constexpr static size_t s_NumFuncs = NumFuncs;

		std::array<Class*, NumBases + 1> bases;
		std::array<Field, NumFields + 1> fields;
		std::array<Function, NumFuncs + 1> functions;
		// Class* bases[];
		//Field fields[NumFields + 1];
		//Function functions[NumFuncs + 1];
	};

}