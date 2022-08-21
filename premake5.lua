include "Dependencies.lua"

workspace "Viking"
    architecture "x86_64"
    startproject "Viking"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    group "Dependencies"
        include "Viking/Vendor/glfw"
    group ""

    include "Sandbox"
    include "Viking"