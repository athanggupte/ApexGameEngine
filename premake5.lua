workspace "ApexGameEngine"
	architecture "x64"
	configurations	{
		"Debug",
		"Release",
		"Dist"
	}
	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "ApexGameEngine"
	location "ApexGameEngine"
	kind "SharedLib"	--DLL
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0.16299.0"

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
		defines "APEX_DEBUG"
		optimize "On"

	filter "configurations:Dist"
		defines "APEX_DEBUG"
		optimize "On"

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
		"ApexGameEngine/vendor/spdlog/include",
		"ApexGameEngine/src"
	}

	links {
		"ApexGameEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "10.0.16299.0"

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
