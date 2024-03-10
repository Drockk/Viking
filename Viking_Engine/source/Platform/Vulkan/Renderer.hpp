#ifndef VULKAN_RENDERER_HPP
#define VULKAN_RENDERER_HPP

#include "Viking/renderer/Context.hpp"

namespace vulkan
{
    class Renderer
    {
    public:
        void init(const std::shared_ptr<vi::Context>& p_context);
        void cleanup();

        void begin_frame();
        void end_frame();
    };
}

#endif // VULKAN_RENDERER_HPP
