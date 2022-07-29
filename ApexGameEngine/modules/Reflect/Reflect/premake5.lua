project "Reflect"
    -- kind "StaticLib"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp",
        "src/**.hpp",
    }

    includedirs {
        "src",
        -- External Dependencies --
        -- "%{wks.location}/bitsery/include",
        "%{IncludeDirs.Utils}",
        "%{IncludeDirs.glm}",
        -- "C:/Users/Rackware_2/Repos/ApexGameEngine/ApexGameEngine/vendor/entt/single_include"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        runtime "Release"
        optimize "on"


    filter "system:windows"
		systemversion "latest"

		defines {
            "APEX_PLATFORM_WINDOWS",
			"REFLECT_DLL",
			"REFLECT_EXPORTS",
		}

        postbuildcommands {
			"%{wks.location}/vendor/bin/defmaker/defmaker.exe %{cfg.linktarget.directory}/%{prj.name}.def %{cfg.linktarget.abspath}",
		}