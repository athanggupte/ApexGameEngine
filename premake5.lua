require "export-compile-commands"

include "dependencies.lua"

workspace "ApexGameEngine"
	architecture "x64"
	configurations	{
		"Debug",
		"Release",
		"Dist"
	}
	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
-- Include other premake files
	include "ApexGameEngine/vendor/GLFW"
	include "ApexGameEngine/vendor/Glad"
	include "ApexGameEngine/vendor/imgui"
	include "ApexGameEngine/vendor/imguizmo_quat"

group "Modules"
	include "ApexGameEngine/modules/Utils"
	include "ApexGameEngine/modules/Reflect"
	include "ApexGameEngine/modules/ApexIK"




linux_de = os.getenv("XDG_CURRENT_DESKTOP")
curDir = os.getenv("CD")
if curDir == nil then
	curDir = os.getenv("PWD")
end
if curDir == nil then
	curDir = '.'
end
curDir = '"'..curDir..'"'


-- Reset group to root
group ""
	include "ApexGameEngine"
	include "ApexEditor"
	include "Sandbox"
	include "ScriptTest"
	-- include "GameLib"


if os.istarget "windows" then
	CMAKE = "%CMAKE_PATH%\\cmake"
	OBJCOPY = "%{wks.location}/vendor/bin/binutils-x64/objcopy.exe"
else
	CMAKE = "cmake"
	OBJCOPY = "objcopy"
end
UTF_BOM_REMOVE = "%{wks.location}/vendor/bin/utf-bom-utils/Debug/bom_remove.exe"

group "Dependencies"
project "Assimp"
	location "ApexGameEngine/vendor/Assimp"
	kind "Makefile"

	buildcommands {
		"{MKDIR} build",
		"{CHDIR} build",
		"%{CMAKE} -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_SAMPLES=OFF -DASSIMP_BUILD_TESTS=OFF ..",
		-- "%{CMAKE} --build . "
	}

	filter "configurations:Debug"
		buildcommands {
			"%{CMAKE} --build . --config Debug"
		}

	filter "configurations:Release or Dist"
		buildcommands {
			"%{CMAKE} --build . --config Release"
		}

	filter "system:windows"
		buildcommands {
			"{ECHO} Copy %{prj.location}/build/bin/Debug/assimp-vc142-mtd.dll to the directory containing the executable binaries after building"
		}

	filter "system:linux"
		buildcommands {
			"{ECHO} Copy %{prj.location}/build/bin/Debug/assimp-vc142-mtd.so to the directory containing the executable binaries after building"
		}

project "FreeType"
	location "ApexGameEngine/vendor/freetype"
	kind "Makefile"

	buildcommands {
		"{MKDIR} build",
		"{CHDIR} build",
		"%{CMAKE} ..",
		-- "%{CMAKE} --build . "
	}

	filter "configurations:Debug"
		buildcommands {
			"%{CMAKE} --build . --config Debug"
		}

	filter "configurations:Release or Dist"
		buildcommands {
			"%{CMAKE} --build . --config Release"
		}

group "Utils"
project "utf-bom-utils"
	location "vendor/utf-bom-utils"
	kind "Makefile"

	buildcommands {
		"{MKDIR} ../bin/%{prj.name}",
		"{CHDIR} ../bin/%{prj.name}",
		"%{CMAKE} ../../%{prj.name}",
		--"%{CMAKE} --build ."
	}

	filter "configurations:Debug"
		buildcommands {
			"%{CMAKE} --build . --config Debug"
		}

	filter "configurations:Release or Dist"
		buildcommands {
			"%{CMAKE} --build . --config Release"
		}