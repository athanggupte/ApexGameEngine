project "Sandbox"
	kind "ConsoleApp"	--Executable
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs {
		"Sandbox/src",
		"ApexGameEngine/src",
		"ApexGameEngine/assets/shaders",
		-- External Dependencies
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.FreeType}",
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
			"APEX_PLATFORM_WINDOWS"
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
        
		-- TODO: Consider replacing with 'runpathdirs' https://premake.github.io/docs/runpathdirs
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
