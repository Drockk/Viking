#include "vipch.hpp"

#include "Viking/Renderer/Context.hpp"

#include "Platform/Vulkan/VulkanContext.hpp"

namespace Viking {

	Ref<Context> Context::create() {
		return createRef<VulkanContext>();
	}
}
