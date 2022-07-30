project "Viking"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

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
        "GLFW"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines { "VI_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"