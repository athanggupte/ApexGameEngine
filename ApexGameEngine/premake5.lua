project "ApexGameEngine"
	kind "StaticLib"	-- StaticLib - LIB / SharedLib - DLL
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "apex_pch.h"
	pchsource "src/apex_pch.cpp"

	files {
		"src/**.h",
		"src/**.cpp",
		"src/**.hpp",
		-- STB Image --
		"%{wks.location}/ApexGameEngine/vendor/stb_image/**.h",
		"%{wks.location}/ApexGameEngine/vendor/stb_image/**.cpp",
		-- GLM --
		"%{wks.location}/ApexGameEngine/vendor/glm/glm/**.hpp",
		"%{wks.location}/ApexGameEngine/vendor/glm/glm/**.inl",
		"%{wks.location}/ApexGameEngine/vendor/glm/glm/**.h",
		-- PugiXML --
		"%{wks.location}/ApexGameEngine/vendor/pugixml/src/**.hpp",
		-- "%{prj.name}/vendor/pugixml/src/**.cpp",
	}

	includedirs {
		"src",
		"assets/shaders",
		-- External Dependencies
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.FreeType}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb_image}",
		"%{wks.location}/ApexGameEngine/vendor/Assimp/build/include",
		"%{IncludeDirs.Assimp}",
		"%{IncludeDirs.irrKlang}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.ImGuizmoQuat}",
		"%{IncludeDirs.pugixml}",
		"%{IncludeDirs.FBX}",
		-- Modules
		"%{IncludeDirs.ApexIK}",
	}

	libdirs {
		--"ApexGameEngine/vendor/Assimp/build/contrib/zlib/Debug",
		--"ApexGameEngine/vendor/Assimp/build/contrib/irrXML/Debug"
		"%{wks.location}/ApexGameEngine/vendor/irrKlang/lib"
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
	
	targetDir = path.getabsolute("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	
	defines {
		"APEX_INSTALL_LOCATION=\""..targetDir.."\"",
		"APEX_USE_VFS",
		-- "APEX_DYNAMIC_LINK" -- Comment if StaticLib
	}
	
	filter "system:windows"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS",
			"APEX_ENGINE_EXPORTS",
			"GLFW_INCLUDE_NONE"
		}
		
		libdirs {
			"%{WinLibDirs.FreeType}",
			"%{WinLibDirs.Assimp}",
			"%{WinLibDirs.irrKlang}",
			"%{WinLibDirs.FBX}",
		}

		links {
			"%{WinLibs.OpenGL}",
			"%{WinLibs.FreeType}",
			"%{WinLibs.Assimp}",
			"%{WinLibs.irrKlang}",
			"%{WinLibs.FBX}",
			"%{WinLibs.FBX_xml}",
			"%{WinLibs.FBX_zlib}",
		}

		postbuildcommands {
			"%{wks.location}/vendor/bin/defmaker/defmaker.exe %{cfg.linktarget.directory}/%{prj.name}.def %{cfg.linktarget.abspath}",
		}
        
	filter "system:linux"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_LINUX",
 			-- "APEX_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}
		
		libdirs {
			"%{LinuxLibDirs.FreeType}",
			"%{LinuxLibDirs.Assimp}",
			"%{LinuxLibDirs.irrKlang}",
		}

		links {
			"%{LinuxLibs.OpenGL}",
			"%{LinuxLibs.FreeType}",
			"%{LinuxLibs.Assimp}",
			"%{LinuxLibs.irrKlang}",
		}
		
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
