#pragma once

#include <random>

namespace Apex {

	class Random
	{
	public:
		static void Init()
		{
			if (std::random_device().entropy()) {
				s_RandomEngine.seed(std::random_device()());
			}
			else {
				APEX_CORE_WARN("True Random Device NOT AVAILABLE");
				s_RandomEngine.seed(std::time(nullptr));
			}
			APEX_CORE_TRACE("Apex::Random initialized succesfully!");
		}

		static float Float()
		{
			return (float)s_Distribution(s_RandomEngine) / (float)s_Distribution.max(); //(float)std::numeric_limits<uint32_t>::max();
		}

		static int Int()
		{
			return s_Distribution(s_RandomEngine);
		}

	private:
		static std::mt19937 s_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
	};

}
