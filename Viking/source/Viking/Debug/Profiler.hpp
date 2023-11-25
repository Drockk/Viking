#pragma once

#ifdef DEBUG
    #define USE_OPTICK 1
    #define OPTICK_ENABLE_TRACING 1
    #define OPTICK_ENABLE_GPU 1
    #define OPTICK_ENABLE_GPU_VULKAN 1
#else
    #define USE_OPTICK 0
#endif

#include <optick.h>

#ifdef DEBUG
    #define PROFILER_APP(x) OPTICK_APP(x)
    #define PROFILER_CATEGORY(x, y) OPTICK_CATEGORY(x, y)
    #define PROFILER_EVENT() OPTICK_EVENT()
    #define PROFILER_FRAME(x) OPTICK_FRAME(x)
// For now Optick doesn't work well because of cleanup
#if 0
    #define PROFILER_GPU_CONTEXT(x) OPTICK_GPU_CONTEXT(x)
    #define PROFILER_GPU_FLIP(x) OPTICK_GPU_FLIP(x)
    #define PROFILER_GPU_INIT_VULKAN(device, physical_device, queue, queue_family) OPTICK_GPU_INIT_VULKAN(device, physical_device, queue, queue_family, 1, nullptr)
#endif
    #define PROFILER_GPU_CONTEXT(x)
    #define PROFILER_GPU_FLIP(x)
    #define PROFILER_GPU_INIT_VULKAN(device, physical_device, queue, queue_family)
#else
    #define PROFILER_APP(x)
    #define PROFILER_CATEGORY(x, y)
    #define PROFILER_EVENT()
    #define PROFILER_FRAME(x)
    #define PROFILER_GPU_CONTEXT(x)
    #define PROFILER_GPU_FLIP(x)
    #define PROFILER_GPU_INIT_VULKAN(device, physical_device, queue, queue_family)
#endif
