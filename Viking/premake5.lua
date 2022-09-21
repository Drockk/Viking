project "Viking"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "vipch.hpp"
    pchsource "Source/vipch.cpp"

    files
    {
        "Source/**.hpp",
        "Source/**.cpp",
        "%{IncludeDir.glm}/glm/**.hpp",
        "%{IncludeDir.glm}/glm/**.inl",
        "%{IncludeDir.stb}/**.h",
        "%{IncludeDir.stb}/**.cpp",
        "%{IncludeDir.tinyObjLoader}/**.h"
    }

    includedirs
    {
        "Source",
        "Vendor/spdlog/include",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.tinyObjLoader}",
        "%{IncludeDir.VulkanSDK}"
    }

    defines
    {
        "GLFW_INCLUDE_VULKAN"
    }

    links
    {
        "GLFW",
        "%{Library.Vulkan}",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "VI_PLATFORM_WINDOWS",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "VI_DEBUG"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines "VI_RELEASE"
        runtime "Release"
        optimize "On"