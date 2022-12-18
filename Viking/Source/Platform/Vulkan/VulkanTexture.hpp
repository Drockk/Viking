#pragma once
#include "Viking/Renderer/Texture.hpp"

#include "Platform/Vulkan/Image.hpp"

namespace Vulkan {
	class Texture2D: public Viking::Texture2D {
	public:
		Texture2D(const std::string& filename);
		~Texture2D() override;

		Viking::Ref<Image>& getImage();
		VkSampler& getSampler();
	private:
		void createTextureSampler(const uint32_t& mipLevels);

		Viking::Ref<Device> m_Device;
		Viking::Ref<Image> m_TextureImage;
		VkSampler m_TextureSampler{};
	};
}
