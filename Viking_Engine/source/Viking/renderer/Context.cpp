#include "Context.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace vi
{
    std::shared_ptr<Context> Context::create()
    {
        return std::make_shared<vulkan::Context>();
    }
}
