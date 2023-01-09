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
		
		-- ImGuizmo --
		"%{wks.location}/ApexGameEngine/vendor/ImGuizmo/ImGuizmo.h",
		"%{wks.location}/ApexGameEngine/vendor/ImGuizmo/ImGuizmo.cpp",
	
		-- Reflect TypeImpl --
		-- "%{wks.location}/ApexGameEngine/modules/Reflect/Reflect/src/TypeImpl.cpp"
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
		"%{IncludeDirs.ImGuizmo}",
		"%{IncludeDirs.pugixml}",
		"%{IncludeDirs.FBX}",
		-- Modules
		"%{IncludeDirs.Utils}",
		"%{IncludeDirs.Reflect}",
		"%{IncludeDirs.ApexIK}",
	}

	includedirs(IncludeDirs["PhysX"])

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
		"Reflect",
		--"zlibd",4"
		--"IrrXMLd",
	}

	targetDir = path.getabsolute("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	
	defines {
		"APEX_INSTALL_LOCATION=\""..targetDir.."\"",
		"APEX_USE_VFS",
		-- "APEX_DYNAMIC_LINK" -- Comment if StaticLib
	}
	
	filter "files:src/Apex/Graphics/ShaderDefines.h"
		buildmessage "Embedding %{file.relpath} into obj"
		buildcommands {
			"%{UTF_BOM_REMOVE} %{file.relpath}",
			"%{OBJCOPY} --input-target binary --output-target pe-x86-64 --binary-architecture i386 %{file.relpath} %{cfg.objdir}/%{file.basename}.obj",
			"{ECHO} Created %{cfg.objdir}/%{file.basename}.obj from %{file.relpath}"
		}
		buildoutputs {
			"%{cfg.objdir}/%{file.basename}.obj"
		}

	filter "files:vendor/ImGuizmo/**.cpp"
		flags { "NoPCH" }

	filter "files:modules/Reflect/**.cpp"
		flags { "NoPCH" }

	filter "system:windows"
		systemversion "latest"

		defines {
			"APEX_PLATFORM_WINDOWS",
			"APEX_ENGINE_EXPORTS",
			"REFLECT_DLL",
			"REFLECT_EXPORTS",
			"GLFW_INCLUDE_NONE",
		}
		
		libdirs {
			"%{WinLibDirs.irrKlang}",
		}

		links {
			"%{WinLibs.OpenGL}",
			"%{WinLibs.irrKlang}",
			"%{WinLibs.FBX}",
			"%{WinLibs.FBX_xml}",
			"%{WinLibs.FBX_zlib}",
		}

		links(WinLibs["PhysX"])

		linkoptions {
			"/ignore:4006",
		}

		postbuildcommands {
			"%{wks.location}/vendor/bin/defmaker/defmaker.exe %{cfg.linktarget.directory}/%{prj.name}.def %{cfg.linktarget.abspath}",
		}
        
	filter { "system:windows", "configurations:Debug" }
		libdirs {
			"%{WinLibDirs.FreeType_Debug}",
			"%{WinLibDirs.Assimp_Debug}",
			"%{WinLibDirs.FBX_Debug}",
			"%{WinLibDirs.PhysX_Debug}",
		}

		links {
			"%{WinLibs.FreeType_Debug}",
			"%{WinLibs.Assimp_Debug}",
		}

	filter { "system:windows", "configurations:Release or Dist" }
		libdirs {
			"%{WinLibDirs.FreeType_Release}",
			"%{WinLibDirs.Assimp_Release}",
			"%{WinLibDirs.FBX_Release}",
			"%{WinLibDirs.PhysX_Release}",
		}

		links {
			"%{WinLibs.FreeType_Release}",
			"%{WinLibs.Assimp_Release}",
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
			"APEX_HOME_DIR="..curDir,
			"_DEBUG"
		}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {
			"APEX_RELEASE", "APEX_PROFILER_ENABLE",
			"APEX_HOME_DIR="..curDir,
			"NDEBUG"
		}
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines { "APEX_DIST", "APEX_HOME_DIR=\".\"", "NDEBUG" }
		runtime "Release"
		optimize "on"
