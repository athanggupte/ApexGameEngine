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

include "ApexGameEngine/vendor/GLFW"
include "ApexGameEngine/vendor/Glad"
include "ApexGameEngine/vendor/imgui"


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
		"%{prj.name}/src/**.cpp",
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
		"%{IncludeDirs.glm}"
	}

	links {
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS",
			"APEX_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines {
			"APEX_DEBUG", "APEX_ENABLE_ASSERTS"
		}
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "APEX_RELEASE"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "APEX_DIST"
		buildoptions "/MD"
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
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}"

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
		defines {
			"APEX_DEBUG", "APEX_ENABLE_ASSERTS"
		}
		buildoptions "/MDd"
		symbols "On"

	filter "configurations:Release"
		defines "APEX_DEBUG"
		buildoptions "/MD"
		optimize "On"

	filter "configurations:Dist"
		defines "APEX_DEBUG"
		buildoptions "/MD"
		optimize "On"
