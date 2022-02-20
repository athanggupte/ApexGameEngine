project "ApexEditor"
	kind "ConsoleApp"	--Executable
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	newoption {
		trigger = "editor-tools",
		description = "Build editor tools like Node Graph"
	}
		
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "apexed_pch.h"
	pchsource "src/apexed_pch.cpp"
	
	files {
		"src/**.h",
		"src/**.cpp"
	}

	if not _OPTIONS["editor-tools"] then
		removefiles {
			"src/EditorTools/**.h",
			"src/EditorTools/**.cpp"
		}
	end

	includedirs {
		"src",
		"%{wks.location}/ApexGameEngine/src",
		"%{wks.location}/ApexGameEngine/assets/shaders",
		-- External Dependencies
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.FreeType}",
		"%{IncludeDirs.glm}",
		"%{wks.location}/ApexGameEngine/vendor/Assimp/build/include",
		"%{IncludeDirs.Assimp}",
		"%{IncludeDirs.irrKlang}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.ImGuizmoQuat}",
		"%{IncludeDirs.ImGuizmo}",
		-- Modules
		"%{IncludeDirs.ApexIK}"
	}

	links {
		"ApexGameEngine",
		-- "GLFW",
		-- "Glad",
		-- "ImGui",
		-- "ImGuizmoQuat",
		-- "ApexIK",
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
			"APEX_ENGINE_EXPORTS"
		}

		linkoptions {
			"/DEF:\"%{wks.location}/bin/" .. outputdir .. "/ApexGameEngine/ApexGameEngine.def\""
		}

		libdirs {
			"%{WinLibDirs.FBX}"
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
