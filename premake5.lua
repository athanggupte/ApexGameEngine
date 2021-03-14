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
IncludeDirs["spdlog"] = "ApexGameEngine/vendor/spdlog/include/"
IncludeDirs["GLFW"] = "ApexGameEngine/vendor/GLFW/include/"
IncludeDirs["Glad"] = "ApexGameEngine/vendor/Glad/include/"
IncludeDirs["ImGui"] = "ApexGameEngine/vendor/imgui/"
IncludeDirs["glm"] = "ApexGameEngine/vendor/glm/"
IncludeDirs["stb_image"] = "ApexGameEngine/vendor/stb_image/"
IncludeDirs["Assimp"] = "ApexGameEngine/vendor/Assimp/src/include/"
IncludeDirs["irrKlang"] = "ApexGameEngine/vendor/irrKlang/include/"
IncludeDirs["ApexIK"] = "ApexGameEngine/vendor/ApexIK/ApexIK/include/"

-- DLLs
DLLs = {}
DLLs["Assimp"] = "assimp-vc142-mtd"

-- Platform library linkages
WinLibs = { "opengl32.lib", "assimp-vc142-mtd", "irrKlang" }
-- WinLibs["OpenGL"] = "opengl32.lib"
-- WinLibs["Assimp"] = "%{DLLs.Assimp}"
-- WinLibs["irrKlang"] = "irrKlang"

WinLibDirs = { "ApexGameEngine/vendor/Assimp/build/code/Debug", "ApexGameEngine/vendor/irrKlang/lib" }
-- WinLibDirs["Assimp"] = "ApexGameEngine/vendor/Assimp/build/code/Debug"
-- WinLibDirs["irrKlang"] = "ApexGameEngine/vendor/irrKlang/lib"

LinuxLibs = { "GL", "dl", "m", "pthread", "assimp", "IrrKlang" }
-- LinuxLibs["OpenGL"] = "GL"
-- LinuxLibs["Assimp"] = "assimp"
-- LinuxLibs["irrKlang"] = "IrrKlang"

LinuxLibDirs = { "ApexGameEngine/vendor/Assimp/build/bin", "ApexGameEngine/vendor/irrKlang/bin/linux-gcc-64" }
-- LinuxLibDirs["Assimp"] = "ApexGameEngine/vendor/Assimp/build/bin"
-- LinuxLibDirs["irrKlang"] = "ApexGameEngine/vendor/irrKlang/bin/linux-gcc-64"

-- Include other premake files
include "ApexGameEngine/vendor/GLFW"
include "ApexGameEngine/vendor/Glad"
include "ApexGameEngine/vendor/imgui"
include "ApexGameEngine/vendor/ApexIK"


-- Apex Game Engine Project
project "ApexGameEngine"
	location "ApexGameEngine"
	kind "StaticLib"	--LIB /SharedLib - DLL
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
		"%{IncludeDirs.stb_image}",
		"%{prj.name}/vendor/Assimp/build/include",
		"%{IncludeDirs.Assimp}",
		"%{IncludeDirs.irrKlang}",
		"%{IncludeDirs.ApexIK}"
	}

	libdirs {
		--"ApexGameEngine/vendor/Assimp/build/contrib/zlib/Debug",
		--"ApexGameEngine/vendor/Assimp/build/contrib/irrXML/Debug"
		"ApexGameEngine/vendor/irrKlang/lib"
	}

	links {
		"GLFW",
		"Glad",
		"ImGui",
		"ApexIK",
		--"zlibd",4"
		--"IrrXMLd",
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS",
			"APEX_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}
		
		libdirs (WinLibDirs)
		links (WinLibs)
        
	filter "system:linux"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_LINUX",
-- 			"APEX_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}
		
		libdirs (LinuxLibDirs)
		links (LinuxLibs)
		
	filter "configurations:Debug"
		defines {
			"APEX_DEBUG", "APEX_ENABLE_ASSERTS", "APEX_PROFILER_ENABLE"
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {
			"APEX_RELEASE", "APEX_PROFILER_ENABLE"
		}
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "APEX_DIST"
		runtime "Release"
		optimize "on"

-- Apex Editor Project

project "ApexEditor"
	location "ApexEditor"
	kind "ConsoleApp"	--Executable
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	newoption {
		trigger = "editor-tools",
		description = "Build editor tools like Node Graph"
	}
		
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
    
	configuration "not editor-tools"
		removefiles {
			"%{prj.name}/src/EditorTools/**.h",
			"%{prj.name}/src/EditorTools/**.cpp"
		}

	includedirs {
		"ApexGameEngine/src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"ApexGameEngine/vendor/Assimp/build/include",
		"%{IncludeDirs.Assimp}",
		"%{IncludeDirs.irrKlang}",
		"%{IncludeDirs.ApexIK}"
	}

	links {
		"ApexGameEngine",
		"GLFW",
		"Glad",
		"ImGui",
		"ApexIK",
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS"
		}
        
		libdirs (WinLibDirs)
		links (WinLibs)
		
	filter "system:linux"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_LINUX"
		}
		
		libdirs (LinuxLibDirs)
		links (LinuxLibs)

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
		"%{IncludeDirs.glm}",
		"ApexGameEngine/vendor/Assimp/build/include",
		"%{IncludeDirs.Assimp}",
		"%{IncludeDirs.irrKlang}",
		"%{IncludeDirs.ApexIK}"
	}

	links {
		"ApexGameEngine",
		"GLFW",
		"Glad",
		"ImGui",
		"ApexIK",
	}

	filter "system:windows"
		staticruntime "On"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS"
		}
		
		libdirs (WinLibDirs)
		links (WinLibs)

	filter "system:linux"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_LINUX"
		}
		
		libdirs (LinuxLibDirs)
		links (LinuxLibs)
		
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
