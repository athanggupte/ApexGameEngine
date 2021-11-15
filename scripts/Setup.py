import os
import SetupPython

SetupPython.ValidatePackages()
SetupPython.ValidateWorkingDir()

import SetupVulkan

# Create Vulkans SDK working directory if not exist
os.makedirs(SetupVulkan.VULKAN_SDK_WORKING_DIR, exist_ok=True)

if not SetupVulkan.CheckVulkanSDK():
    print('Vulkan SDK not installed.')
    
if not SetupVulkan.CheckVulkanSDKDebugLibs():
    print('Vulkan SDK debug libs not installed.')
