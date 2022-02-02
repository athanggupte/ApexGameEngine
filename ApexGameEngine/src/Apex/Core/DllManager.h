#pragma once

#include <filesystem>
namespace fs = std::filesystem;

namespace Apex {

	class Dll
	{
	public:
		virtual ~Dll() = default;
		[[nodiscard]] virtual void* GetSymbolAddress(const std::string& symbol) const = 0;

	protected:
		Dll() = default;
	};

	Dll* LoadDll(const fs::path& filepath);

}
