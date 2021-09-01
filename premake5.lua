workspace "ApexGameEngine"
	architecture "x64"
	configurations	{
		"Debug",
		"Release",
		"Dist"
	}
	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

WinCRunTime_Type = "Off"

-- Include directories relative to the root (solution) directory
IncludeDirs = {}
IncludeDirs["spdlog"] = "ApexGameEngine/vendor/spdlog/include/"
IncludeDirs["GLFW"] = "ApexGameEngine/vendor/GLFW/include/"
IncludeDirs["Glad"] = "ApexGameEngine/vendor/Glad/include/"
IncludeDirs["ImGui"] = "ApexGameEngine/vendor/imgui/"
IncludeDirs["glm"] = "ApexGameEngine/vendor/glm/"
IncludeDirs["stb_image"] = "ApexGameEngine/vendor/stb_image/"
IncludeDirs["Assimp"] = "ApexGameEngine/vendor/Assimp/include/"
IncludeDirs["irrKlang"] = "ApexGameEngine/vendor/irrKlang/include/"
IncludeDirs["entt"] = "ApexGameEngine/vendor/entt/single_include/"
IncludeDirs["pugixml"] = "ApexGameEngine/vendor/pugixml/src/"
IncludeDirs["ImGuizmoQuat"] = "ApexGameEngine/vendor/imguizmo_quat/imGuIZMO.quat/"
IncludeDirs["ApexIK"] = "ApexGameEngine/modules/ApexIK/ApexIK/include/"

-- DLLs
DLLs = {}
DLLs["Assimp"] = "assimp-vc142-mtd"

-- Platform library linkages
WinLibs = { "opengl32.lib", "assimp-vc142-mtd", "irrKlang" }
-- WinLibs["OpenGL"] = "opengl32.lib"
-- WinLibs["Assimp"] = "%{DLLs.Assimp}"
-- WinLibs["irrKlang"] = "irrKlang"

WinLibDirs = { "ApexGameEngine/vendor/Assimp/build/lib/Debug", "ApexGameEngine/vendor/irrKlang/lib" }
-- WinLibDirs["Assimp"] = "ApexGameEngine/vendor/Assimp/build/code/Debug"
-- WinLibDirs["irrKlang"] = "ApexGameEngine/vendor/irrKlang/lib"

LinuxLibs = { "GL", "dl", "m", "pthread", "uuid", "assimp", "IrrKlang" }
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
include "ApexGameEngine/vendor/imguizmo_quat"

include "ApexGameEngine/modules/ApexIK"

local linux_de = os.getenv("XDG_CURRENT_DESKTOP")
local curDir = os.getenv("CD")
if curDir == nil then
	curDir = os.getenv("PWD")
end
if curDir == nil then
	curDir = '.'
end
curDir = '"'..curDir..'"'

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
		-- STB Image --
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		-- GLM --
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/glm/glm/**.h",
		-- PugiXML --
		"%{prj.name}/vendor/pugixml/src/**.hpp",
		-- "%{prj.name}/vendor/pugixml/src/**.cpp",
	}

	includedirs {
		"%{prj.name}/src",
		-- External Dependencies
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb_image}",
		"%{prj.name}/vendor/Assimp/build/include",
		"%{IncludeDirs.Assimp}",
		"%{IncludeDirs.irrKlang}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.ImGuizmoQuat}",
		"%{IncludeDirs.pugixml}",
		-- Modules
		"%{IncludeDirs.ApexIK}",
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
		"ImGuizmoQuat",
		"ApexIK",
		--"zlibd",4"
		--"IrrXMLd",
	}
	
	targetDir = path.getabsolute("bin/" .. outputdir .. "/%{prj.name}")
	
	defines {
		"APEX_INSTALL_LOCATION=\""..targetDir.."\"",
		"APEX_USE_VFS"
	}
	
	filter "system:windows"
		systemversion "latest"
		staticruntime(WinCRunTime_Type)

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
		
		if linux_de == "KDE" then
			defines { "LINUX_DE_KDE" }
		elseif linux_de == "GNOME" then
			defines { "LINUX_DE_GNOME" }
		elseif linux_de == "lxqt" then
			defines { "LINUX_DE_LXQT" }
		end
		
	filter "configurations:Debug"
		defines {
			"APEX_DEBUG", "APEX_ENABLE_ASSERTS", "APEX_PROFILER_ENABLE",
			"APEX_HOME_DIR="..curDir
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {
			"APEX_RELEASE", "APEX_PROFILER_ENABLE",
			"APEX_HOME_DIR="..curDir
		}
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines { "APEX_DIST", "APEX_HOME_DIR=\".\"" }
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

	pchheader "apexed_pch.h"
	pchsource "ApexEditor/src/apexed_pch.cpp"
	
	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	if not _OPTIONS["editor-tools"] then
		removefiles {
			"%{prj.name}/src/EditorTools/**.h",
			"%{prj.name}/src/EditorTools/**.cpp"
		}
	end

	includedirs {
		"ApexEditor/src",
		"ApexGameEngine/src",
		-- External Dependencies
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"ApexGameEngine/vendor/Assimp/build/include",
		"%{IncludeDirs.Assimp}",
		"%{IncludeDirs.irrKlang}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.ImGuizmoQuat}",
		-- Modules
		"%{IncludeDirs.ApexIK}"
	}

	links {
		"ApexGameEngine",
		"GLFW",
		"Glad",
		"ImGui",
		"ImGuizmoQuat",
		"ApexIK",
	}

	targetDir = path.getabsolute("bin/" .. outputdir .. "/%{prj.name}")
	
	defines {
		"APEX_INSTALL_LOCATION=\""..targetDir.."\"",
		"APEX_USE_VFS"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime(WinCRunTime_Type)

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

		postbuildcommands {
			"echo \"cd $(realpath %{cfg.buildtarget.directory}) && export LD_LIBRARY_PATH=/home/alamar213/Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/bin:/home/alamar213/Work/ApexGameEngine/ApexGameEngine/vendor/irrKlang/bin/linux-gcc-64/ && ./%{prj.name}\" > %{cfg.buildtarget.abspath}.sh"
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
		"Sandbox/src",
		"ApexGameEngine/src",
		-- External Dependencies
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"ApexGameEngine/vendor/Assimp/build/include",
		"%{IncludeDirs.Assimp}",
		"%{IncludeDirs.irrKlang}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.ImGuizmoQuat}",
		-- Modules
		"%{IncludeDirs.ApexIK}"
	}

	links {
		"ApexGameEngine",
		"GLFW",
		"Glad",
		"ImGui",
		"ImGuizmoQuat",
		"ApexIK",
	}
	
	targetDir = path.getabsolute("bin/" .. outputdir .. "/%{prj.name}")
	
	defines {
		"APEX_INSTALL_LOCATION=\""..targetDir.."\"",
		"APEX_USE_VFS"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime(WinCRunTime_Type)

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
        
		postbuildcommands {
			"echo \"cd $(realpath %{cfg.buildtarget.directory}) && export LD_LIBRARY_PATH=/home/alamar213/Work/ApexGameEngine/ApexGameEngine/vendor/Assimp/build/bin:/home/alamar213/Work/ApexGameEngine/ApexGameEngine/vendor/irrKlang/bin/linux-gcc-64/ && ./%{prj.name}\" > %{cfg.buildtarget.abspath}.sh"
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
		defines "APEX_DIST"
		optimize "on"



project "Assimp"
	location "ApexGameEngine/vendor/Assimp"
	kind "Makefile"

	buildcommands {
		"{MKDIR} build",
		"{CHDIR} build",
		"cmake -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_SAMPLES=OFF -DASSIMP_BUILD_TESTS=OFF ..",
		"cmake --build . "
	}

	filter "system:windows"
		buildcommands {
			"{ECHO} Copy %{prj.location}/build/bin/Debug/assimp-vc142-mtd.dll to the directory containing the executable binaries after building"
		}

	filter "system:linux"
		buildcommands {
			"{ECHO} Copy %{prj.location}/build/bin/Debug/assimp-vc142-mtd.so to the directory containing the executable binaries after building"
		}