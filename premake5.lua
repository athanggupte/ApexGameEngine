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
IncludeDirs["Glad"] = "ApexGameEngine/vendor/Glad/include"
IncludeDirs["ImGui"] = "ApexGameEngine/vendor/imgui"
IncludeDirs["glm"] = "ApexGameEngine/vendor/glm"
IncludeDirs["stb_image"] = "ApexGameEngine/vendor/stb_image"

include "ApexGameEngine/vendor/GLFW"
include "ApexGameEngine/vendor/Glad"
include "ApexGameEngine/vendor/imgui"


-- Apex Game Engine Project
project "ApexGameEngine"
	location "ApexGameEngine"
	kind "StaticLib"	--DLL
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "apex_pch.h"
	pchsource "ApexGameEngine/src/apex_pch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/glm/glm/**.h"
	}

	includedirs {
		"%{prj.name}/src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb_image}"
	}

	links {
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS",
			"APEX_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines {
			"APEX_DEBUG", "APEX_ENABLE_ASSERTS"
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "APEX_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "APEX_DIST"
		runtime "Release"
		optimize "on"


-- Client Application Project
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"	--Executable
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"ApexGameEngine/src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}"

	}

	links {
		"ApexGameEngine"
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines {
			"APEX_DEBUG", "APEX_ENABLE_ASSERTS"
		}
		symbols "on"

	filter "configurations:Release"
		defines "APEX_RELEASE"
		optimize "on"

	filter "configurations:Dist"
		defines "APEX_RELEASE"
		optimize "on"
