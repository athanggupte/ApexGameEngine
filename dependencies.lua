FBX_SDK = os.getenv("FBX_SDK_PATH")
printf("Found FBX SDK at %s", FBX_SDK)
PHYSX_PATH = "%{wks.location}/ApexGameEngine/vendor/PhysX"

-- Include directories relative to the root (solution) directory
IncludeDirs = {}
IncludeDirs["spdlog"] =       "%{wks.location}/ApexGameEngine/vendor/spdlog/include/"
IncludeDirs["GLFW"] =         "%{wks.location}/ApexGameEngine/vendor/GLFW/include/"
IncludeDirs["Glad"] =         "%{wks.location}/ApexGameEngine/vendor/Glad/include/"
IncludeDirs["ImGui"] =        "%{wks.location}/ApexGameEngine/vendor/imgui/"
IncludeDirs["FreeType"] =     "%{wks.location}/ApexGameEngine/vendor/freetype/include"
IncludeDirs["glm"] =          "%{wks.location}/ApexGameEngine/vendor/glm/"
IncludeDirs["stb_image"] =    "%{wks.location}/ApexGameEngine/vendor/stb_image/"
IncludeDirs["Assimp"] =       "%{wks.location}/ApexGameEngine/vendor/Assimp/include/"
IncludeDirs["irrKlang"] =     "%{wks.location}/ApexGameEngine/vendor/irrKlang/include/"
IncludeDirs["entt"] =         "%{wks.location}/ApexGameEngine/vendor/entt/single_include/"
IncludeDirs["pugixml"] =      "%{wks.location}/ApexGameEngine/vendor/pugixml/src/"
IncludeDirs["ImGuizmoQuat"] = "%{wks.location}/ApexGameEngine/vendor/imguizmo_quat/imGuIZMO.quat/"
IncludeDirs["ImGuizmo"] =     "%{wks.location}/ApexGameEngine/vendor/ImGuizmo/"
IncludeDirs["Utils"] =        "%{wks.location}/ApexGameEngine/modules/Utils/include/"
IncludeDirs["ApexIK"] =       "%{wks.location}/ApexGameEngine/modules/ApexIK/ApexIK/include/"
IncludeDirs["Reflect"] =      "%{wks.location}/ApexGameEngine/modules/Reflect/Reflect/src/"
IncludeDirs["FBX"] =          "%{FBX_SDK}/include"
IncludeDirs["PhysX"] = {
	"%{PHYSX_PATH}/physx/include",
	"%{PHYSX_PATH}/pxshared/include",
}

-- Platform library linkages
-- Windows
WinLibDirs = {}
WinLibDirs["FreeType_Debug"] = "%{wks.location}/ApexGameEngine/vendor/freetype/build/Debug"
WinLibDirs["Assimp_Debug"] =   "%{wks.location}/ApexGameEngine/vendor/Assimp/build/lib/Debug"
WinLibDirs["FBX_Debug"] =      "%{FBX_SDK}/lib/vs2019/x64/debug"
WinLibDirs["PhysX_Debug"] =    "%{PHYSX_PATH}/physx/bin/win.x86_64.vc142.md/debug"

WinLibDirs["FreeType_Release"] = "%{wks.location}/ApexGameEngine/vendor/freetype/build/Release"
WinLibDirs["Assimp_Release"] =   "%{wks.location}/ApexGameEngine/vendor/Assimp/build/lib/Release"
WinLibDirs["FBX_Release"] =      "%{FBX_SDK}/lib/vs2019/x64/release"
WinLibDirs["irrKlang"] =         "%{wks.location}/ApexGameEngine/vendor/irrKlang/lib"
WinLibDirs["PhysX_Release"] =    "%{PHYSX_PATH}/physx/bin/win.x86_64.vc142.md/checked"

WinLibs = {}
WinLibs["FreeType_Debug"] = "freetyped.lib"
WinLibs["Assimp_Debug"] =   "assimp-vc143-mtd"

WinLibs["FreeType_Release"] = "freetype.lib"
WinLibs["Assimp_Release"] =   "assimp-vc143-mt"

WinLibs["OpenGL"] =   "opengl32.lib"
WinLibs["irrKlang"] = "irrKlang"
WinLibs["FBX"] =      "libfbxsdk-md.lib"
WinLibs["FBX_xml"] =  "libxml2-md.lib"
WinLibs["FBX_zlib"] = "zlib-md.lib"
WinLibs["PhysX"] = {
	"PhysX_static_64.lib",
	"PhysXFoundation_static_64.lib",
	"PhysXCommon_static_64.lib",
	"PhysXCooking_static_64.lib",
	"PhysXExtensions_static_64.lib",
	"PhysXCharacterKinematic_static_64.lib",
	"PhysXVehicle_static_64.lib",
	"PhysXPvdSDK_static_64.lib",
}

-- Linux
LinuxLibDirs = {}
LinuxLibDirs["FreeType"] = "%{wks.location}/ApexGameEngine/vendor/freetype/build/Debug"
LinuxLibDirs["Assimp"] =   "%{wks.location}/ApexGameEngine/vendor/Assimp/build/bin"
LinuxLibDirs["irrKlang"] = "%{wks.location}/ApexGameEngine/vendor/irrKlang/bin/linux-gcc-64"

LinuxLibs = { "dl", "m", "pthread", "uuid" }
LinuxLibs["OpenGL"] = "GL"
LinuxLibs["FreeType"] = "freetyped"
LinuxLibs["Assimp"] = "assimp"
LinuxLibs["irrKlang"] = "IrrKlang"
