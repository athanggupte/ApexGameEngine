#pragma once

namespace Apex {

	template<typename T>
	class ResourceFactory
	{
	public:
		Ref<T> Build() { static_assert("Unknown resource type!"); }
	};

}
