#include "vipch.hpp"
#include "Platform/Vulkan/VulkanTexture.hpp"

#include "Platform/Vulkan/Buffer.hpp"
#include "Platform/Vulkan/Context.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <vulkan/vulkan.hpp>

#include "Viking/Renderer/Renderer.hpp"

namespace Vulkan {
    Texture2D::Texture2D(const std::string& filename) {
        m_Device = Context::get()->getDevice();

        auto textureWidth{ 0 }, textureHeight{ 0 }, textureChannels{ 0 };
        auto* pixels = stbi_load(filename.c_str(), &textureWidth, &textureHeight, &textureChannels, STBI_rgb_alpha);
        const VkDeviceSize imageSize = static_cast<VkDeviceSize>(textureWidth) * textureHeight * 4;
        auto mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(textureWidth, textureHeight)))) + 1;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        auto stagingBuffer = Buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        void* data;
        const auto device = Context::get()->getDevice()->get();
        vkMapMemory(device, stagingBuffer.getBufferMemory(), 0, imageSize, 0, &data);
        memcpy(data, pixels, imageSize);
        vkUnmapMemory(device, stagingBuffer.getBufferMemory());

        stbi_image_free(pixels);

        m_TextureImage = Viking::createRef<Image>(textureWidth, textureHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

        m_TextureImage->transitionImageLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

        m_TextureImage->copyBufferToImage(stagingBuffer.getBuffer(), static_cast<uint32_t>(textureWidth), static_cast<uint32_t>(textureHeight));

        m_TextureImage->generateMipmaps(VK_FORMAT_R8G8B8A8_SRGB, textureWidth, textureHeight, mipLevels);

        m_TextureImage->createImageView(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);

        createTextureSampler(mipLevels);
    }

    Texture2D::~Texture2D() {
        vkDestroySampler(m_Device->get(), m_TextureSampler, nullptr);
    }

    Viking::Ref<Image>& Texture2D::getImage() {
        return m_TextureImage;
    }

    VkSampler& Texture2D::getSampler() {
        return m_TextureSampler;
    }

    void Texture2D::createTextureSampler(const uint32_t& mipLevels) {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(Context::get()->getPhysicalDevice()->get(), &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(m_Device->get(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
}
