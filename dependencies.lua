-- Viking dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["glfw"] = "%{wks.location}/Viking/Vendor/glfw/include"
IncludeDir["glm"] = "%{wks.location}/Viking/Vendor/glm"
IncludeDir["stb"] = "%{wks.location}/Viking/Vendor/stb/include"
IncludeDir["tinyObjLoader"] = "%{wks.location}/Viking/Vendor/tiny_obj_loader/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
