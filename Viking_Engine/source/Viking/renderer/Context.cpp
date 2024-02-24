#include "Context.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace vi
{
    std::unique_ptr<Context> Context::create()
    {
        return std::make_unique<vulkan::Context>();
    }
}
