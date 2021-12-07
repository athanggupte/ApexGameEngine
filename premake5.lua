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
	-- include "GameLib"


if os.istarget "windows" then
	CMAKE = "%CMAKE_PATH%\\cmake"
else
	CMAKE = "cmake"
end

group "Dependencies"
project "Assimp"
	location "ApexGameEngine/vendor/Assimp"
	kind "Makefile"

	buildcommands {
		"{MKDIR} build",
		"{CHDIR} build",
		"%{CMAKE} -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_SAMPLES=OFF -DASSIMP_BUILD_TESTS=OFF ..",
		"%{CMAKE} --build . "
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
		"%{CMAKE} --build . "
	}