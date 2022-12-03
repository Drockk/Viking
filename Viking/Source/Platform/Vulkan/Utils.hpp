#pragma once

#include <vulkan/vulkan.hpp>

namespace Vulkan {
	const std::vector VALIDATION_LAYERS = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector DEVICE_EXTENSIONS = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
}
