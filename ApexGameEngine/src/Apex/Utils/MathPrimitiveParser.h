#pragma once

#include <glm/matrix.hpp>
#include <glm/glm.hpp>

#include <glm/gtx/string_cast.hpp>

namespace Apex {

	namespace MathParser {
		
		template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q = glm::defaultp>
		const std::string ParseMatrix(const glm::mat<C, R, T, Q>& matrix)
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
