FBX_SDK = os.getenv("FBX_SDK_PATH")

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
IncludeDirs["ApexIK"] =       "%{wks.location}/ApexGameEngine/modules/ApexIK/ApexIK/include/"
IncludeDirs["FBX"] =          "%{FBX_SDK}/include"

-- Platform library linkages
-- Windows
WinLibDirs = {}
WinLibDirs["FreeType"] =  "%{wks.location}/ApexGameEngine/vendor/freetype/build/Debug"
WinLibDirs["Assimp"] =    "%{wks.location}/ApexGameEngine/vendor/Assimp/build/lib/Debug"
WinLibDirs["irrKlang"] =  "%{wks.location}/ApexGameEngine/vendor/irrKlang/lib"
WinLibDirs["FBX"] =       "%{FBX_SDK}/lib/vs2019/x64/debug"

WinLibs = {}
WinLibs["OpenGL"] =   "opengl32.lib"
WinLibs["FreeType"] = "freetyped"
WinLibs["Assimp"] =   "assimp-vc142-mtd"
WinLibs["irrKlang"] = "irrKlang"
WinLibs["FBX"] =      "libfbxsdk-md.lib"
WinLibs["FBX_xml"] =  "libxml2-md.lib"
WinLibs["FBX_zlib"] = "zlib-md.lib"

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
