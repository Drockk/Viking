#include "vipch.hpp"

#include "Viking/Renderer/Pipeline.hpp"

#include "Platform/Vulkan/VulkanPipeline.hpp"

namespace Viking {

	Ref<Pipeline> Pipeline::create() {
		return createRef<VulkanPipeline>();
	}
}
