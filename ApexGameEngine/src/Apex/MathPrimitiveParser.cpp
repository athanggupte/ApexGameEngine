#include "apex_pch.h"
#include "MathPrimitiveParser.h"

#include <glm/gtx/string_cast.hpp>

namespace Apex {

	namespace MathParser {
		const std::string ParseMatrix(const glm::mat4& matrix)
		{
			const std::string toReplace = "), ";
			const std::string replaceStr = ")\n\t\t\t";

			std::string str = glm::to_string(matrix);
			size_t pos = str.find(toReplace);

			while (pos != std::string::npos) {
				str.replace(pos, toReplace.size(), replaceStr);

				pos = str.find(toReplace, pos + replaceStr.size());
			}

			return str;
		}
	}
}