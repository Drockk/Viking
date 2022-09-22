project "Sandbox"
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
        "%{wks.location}/Viking/Vendor/spdlog/include",
        "%{wks.location}/Viking/Source",
        "%{wks.location}/Viking/Vendor",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.tinyObjLoader}",
        "%{IncludeDir.VulkanSDK}"
    }

    links
    {
        "Viking"
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
        symbols "on"

    filter "configurations:Release"
        defines "VI_RELEASE"
        runtime "Release"
        optimize "on"
