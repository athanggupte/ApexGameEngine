import sys
import os
import requests
from zipfile import ZipFile

import Utils

# From github.com/TheCherno/Hazel

VULKAN_SDK = os.environ.get('VULKAN_SDK')
VULKAN_SDK_VERSION = '1.2.189.0'
VULKAN_SDK_INSTALLER_URL = f'https://sdk.lunarg.com/sdk/download/{VULKAN_SDK_VERSION}/windows/VulkanSDK-{VULKAN_SDK_VERSION}-Installer.exe'
VULKAN_SDK_WORKING_DIR = 'ApexGameEngine/vendor/VulkanSDK/'
VULKAN_SDK_INSTALLER_EXE = f'{VULKAN_SDK_WORKING_DIR}/VulkanSDK-Installer.exe'
VULKAN_SDK_DEBUGLIBS_URL = f'https://sdk.lunarg.com/sdk/download/{VULKAN_SDK_VERSION}/windows/VulkanSDK-{VULKAN_SDK_VERSION}-DebugLibs.zip'
VULKAN_SDK_DEBUGLIBS_TMP_FILE = f'{VULKAN_SDK_WORKING_DIR}/VulkanSDK-DebugLibs.zip'

def InstallVulkanSDK():
    print(f"Downloading Vulkan from '{VULKAN_SDK_INSTALLER_URL}' to {VULKAN_SDK_INSTALLER_EXE}")
    Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_INSTALLER_EXE)
    print('Done!')

    print('Running Vulkan SDK installer...')
    os.startfile(os.path.abspath(VULKAN_SDK_INSTALLER_EXE))
    print('Re-run this script after installation')

def InstallVulkanSDKPrompt():
    print('Would you like to install the Vulkan SDK?')
    if Utils.YesOrNo():
        InstallVulkanSDK()
        quit()

def CheckVulkanSDK():
    if VULKAN_SDK is None:
        print('Vulkan SDK is not installed!')
        InstallVulkanSDKPrompt()
        return False
    elif VULKAN_SDK_VERSION not in VULKAN_SDK:
        print(f'Found Vulkan SDK at {VULKAN_SDK}. But the installed version is NOT COMPATIBLE.')
        print(f'Required version: {VULKAN_SDK_VERSION}')
        InstallVulkanSDKPrompt()
        return False
    
    print(f'Found compatible Vulkan SDK installed at {VULKAN_SDK}')
    return True

def CheckVulkanSDKDebugLibs():
    shaderc_debuglib = os.path.abspath(f'{VULKAN_SDK_WORKING_DIR}/Lib/shaderc_sharedd.lib')
    if not os.path.exists(shaderc_debuglib):
        print(f'Vulkan SDK debug libs not found. Checked {shaderc_debuglib}')
        print(f"Downloading from '{VULKAN_SDK_DEBUGLIBS_URL}'...")
        Utils.DownloadFile(VULKAN_SDK_DEBUGLIBS_URL, VULKAN_SDK_DEBUGLIBS_TMP_FILE)
        print(f'Extracting to {VULKAN_SDK_WORKING_DIR}...')
        with ZipFile(os.path.abspath(VULKAN_SDK_DEBUGLIBS_TMP_FILE)) as zfile:
            zfile.extractall(VULKAN_SDK_WORKING_DIR)
        os.remove(os.path.abspath(VULKAN_SDK_DEBUGLIBS_TMP_FILE))

    print(f'Found Vulkan SDK debug libs at {VULKAN_SDK_WORKING_DIR}')
    return True
