#include "vipch.hpp"

#include "Viking/Renderer/Context.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"

namespace Viking {

	Scope<Context> Context::create() {
		return createScope<VulkanContext>();
	}
}
