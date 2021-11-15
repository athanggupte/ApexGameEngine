VULKAN_SDK = os.getenv("VULKAN_SDK")

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
IncludeDirs["ApexIK"] =       "%{wks.location}/ApexGameEngine/modules/ApexIK/ApexIK/include/"
IncludeDirs["VulkanSDK"] =    "%{VULKAN_SDK}/Include"

-- Platform library linkages
-- Windows
WinLibDirs = {}
WinLibDirs["FreeType"] =  "%{wks.location}/ApexGameEngine/vendor/freetype/build/Debug"
WinLibDirs["Assimp"] =    "%{wks.location}/ApexGameEngine/vendor/Assimp/build/lib/Debug"
WinLibDirs["irrKlang"] =  "%{wks.location}/ApexGameEngine/vendor/irrKlang/lib"
WinLibDirs["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
WinLibDirs["VulkanSDK_Debug"] = "%{wks.location}/ApexGameEngine/vendor/VulkanSDK/Lib"

WinLibs = {}
WinLibs["OpenGL"] =   "opengl32.lib"
WinLibs["FreeType"] = "freetyped"
WinLibs["Assimp"] =   "assimp-vc142-mtd"
WinLibs["irrKlang"] = "irrKlang"

WinLibs["Vulkan"] =   "%{WinLibDirs.VulkanSDK}/vulkan-1.lib"
WinLibs["VulkanUtils"] = "%{WinLibDirs.VulkanSDK}/VkLayer_utils.lib"

WinLibs["ShaderC_Debug"] = "%{WinLibDirs.VulkanSDK_Debug}/shaderc_sharedd.lib"
WinLibs["SPIRV_Cross_Debug"] = "%{WinLibDirs.VulkanSDK_Debug}/spirv-cross-cored.lib"
WinLibs["SPIRV_Cross_GLSL_Debug"] = "%{WinLibDirs.VulkanSDK_Debug}/spirv-cross-glsld.lib"
WinLibs["SPIRV_Tools_Debug"] = "%{WinLibDirs.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

WinLibs["ShaderC_Release"] = "%{WinLibDirs.VulkanSDK}/shaderc_shared.lib"
WinLibs["SPIRV_Cross_Release"] = "%{WinLibDirs.VulkanSDK}/spirv-cross-core.lib"
WinLibs["SPIRV_Cross_GLSL_Release"] = "%{WinLibDirs.VulkanSDK}/spirv-cross-glsl.lib"

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
