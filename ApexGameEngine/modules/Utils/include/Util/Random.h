#pragma once

#include <random>

//#define APEX_USE_DOUBLE_PRECISION

namespace Apex {

	class APEX_API Random
	{
	#ifdef APEX_USE_DOUBLE_PRECISION
		using mt_engine = std::mt19937_64;
	#else
		using mt_engine = std::mt19937;
	#endif

	public:
		static void Init()
		{
			if (std::random_device().entropy() > 0) {
				s_RandomEngine.seed(std::random_device()());
			}
			else {
				APEX_CORE_WARN("True Random Device NOT AVAILABLE");
				s_RandomEngine.seed(static_cast<mt_engine::result_type>(std::time(nullptr)));
			}
			APEX_CORE_TRACE("Apex::Random initialized succesfully!");
		}

		static float Float()
		{
			return static_cast<float>(s_Distribution(s_RandomEngine)) / static_cast<float>(s_Distribution.max()); //(float)std::numeric_limits<uint32_t>::max();
		}

		static int Int()
		{
			return static_cast<int>(s_Distribution(s_RandomEngine));
		}

	private:
		inline static mt_engine s_RandomEngine;
		inline static std::uniform_int_distribution<mt_engine::result_type> s_Distribution;
	};

}
