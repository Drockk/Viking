#include "vipch.hpp"
#include "Viking/Renderer/Texture.hpp"
#include "Platform/Vulkan/VulkanTexture.hpp"

namespace Viking {
	Ref<Texture2D> Texture2D::create(const std::string& filename) {
		return createRef<Vulkan::Texture2D>(filename);
	}
}
