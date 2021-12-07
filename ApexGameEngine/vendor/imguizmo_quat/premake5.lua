project "ImGuizmoQuat"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir)
	objdir ("bin-int/" .. outputdir)

	srclocation = "imGuIZMO.quat"
	
	files
	{
		srclocation.."/imGuIZMOquat.cpp",
		srclocation.."/imGuIZMOquat.h",
		srclocation.."/vgConfig.h",
		srclocation.."/vGizmo.h",
		srclocation.."/vGizmoMath.h",
		srclocation.."/vgMath.h"
	}

	includedirs {
		srclocation.."/include",
		"../glm",
		"../imgui"
	}
	
	filter "system:windows"
		systemversion "latest"
	
	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
