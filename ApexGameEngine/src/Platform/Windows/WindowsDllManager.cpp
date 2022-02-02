#include "apex_pch.h"
#include "Apex/Core/DllManager.h"

#ifdef APEX_PLATFORM_WINDOWS

namespace Apex {

	class WindowsDll final : public Dll
	{
	public:
		WindowsDll(HMODULE library)
			: Dll(), m_Library(library)
		{
		}

		~WindowsDll() override
		{
			FreeLibrary(m_Library);
		}

		[[nodiscard]] void* GetSymbolAddress(const std::string& symbol) const override
		{
			return (void*)GetProcAddress(m_Library, symbol.c_str());
		}

	private:
		HMODULE m_Library;
	};


	Dll* LoadDll(const fs::path& filepath)
	{
		if (HMODULE library = LoadLibrary(filepath.c_str())) {
			return new WindowsDll(library);
		}
		APEX_CORE_CRITICAL("'{0}' could not be loaded!", filepath);
		return nullptr;
	}
}

#endif // APEX_PLATFORM_WINDOWS
