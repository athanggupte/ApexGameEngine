project "ScriptTest"
	kind "SharedLib"	--DLL
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
		
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "src/pch.cpp"

	files {
		"src/**.h",
		"src/**.cpp"
	}

	excludes {
		"**.reflect.cpp"
	}

	includedirs {
		"src",
		"%{wks.location}/ApexGameEngine/src",
		"%{wks.location}/ApexGameEngine/assets/shaders",
		-- External Dependencies
		"%{IncludeDirs.spdlog}",
		-- "%{IncludeDirs.ImGui}",
		-- "%{IncludeDirs.FreeType}",
		"%{IncludeDirs.glm}",
		-- "%{wks.location}/ApexGameEngine/vendor/Assimp/build/include",
		-- "%{IncludeDirs.Assimp}",
		-- "%{IncludeDirs.irrKlang}",
		"%{IncludeDirs.entt}",
		-- "%{IncludeDirs.ImGuizmoQuat}",
		-- Modules
		"%{IncludeDirs.Utils}",
		"%{IncludeDirs.Reflect}",
		"%{IncludeDirs.ApexIK}",
	}

	links {
		"%{wks.location}/bin/" .. outputdir .. "/ApexEditor/ApexEditor.lib",
		"Reflect",
		-- "GLFW",
		-- "Glad",
		-- "ImGui",
		-- "ImGuizmoQuat",
		-- "ApexIK",
	}

	dependson {
		"ApexEditor",
	}

	targetDir = path.getabsolute("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	
	defines {
		"APEX_INSTALL_LOCATION=\""..targetDir.."\"",
		"APEX_USE_VFS"
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS",
			"SCRIPT_TEST_EXPORTS"
		}

		-- libdirs (WinLibDirs)
		-- links (WinLibs)
		
	filter "system:linux"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_LINUX"
		}
		
		-- libdirs (LinuxLibDirs)
		-- links (LinuxLibs)
		
	filter "configurations:Debug"
		defines {
			"APEX_DEBUG", "APEX_ENABLE_ASSERTS"
		}
		symbols "on"

	filter "configurations:Release"
		defines "APEX_RELEASE"
		optimize "on"

	filter "configurations:Dist"
		defines "APEX_DIST"
		optimize "on"
