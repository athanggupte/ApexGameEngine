workspace "ApexGameEngine"
	architecture "x64"
	configurations	{
		"Debug",
		"Release",
		"Dist"
	}
	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to the root (solution) directory
IncludeDirs = {}
IncludeDirs["spdlog"] = "ApexGameEngine/vendor/spdlog/include"
IncludeDirs["GLFW"] = "ApexGameEngine/vendor/GLFW/include"

include "ApexGameEngine/vendor/GLFW"


-- Apex Game Engine Project
project "ApexGameEngine"
	location "ApexGameEngine"
	kind "SharedLib"	--DLL
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "apex_pch.h"
	pchsource "ApexGameEngine/src/apex_pch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"%{prj.name}/src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.GLFW}"
	}

	links {
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS",
			"APEX_BUILD_DLL"
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "APEX_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "APEX_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "APEX_DIST"
		optimize "On"


-- Client Application Project
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"	--Executable
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"ApexGameEngine/src",
		"%{IncludeDirs.spdlog}"
	}

	links {
		"ApexGameEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "APEX_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "APEX_DEBUG"
		optimize "On"

	filter "configurations:Dist"
		defines "APEX_DEBUG"
		optimize "On"
