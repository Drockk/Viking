#pragma once

#include "Platform/Vulkan/Device.hpp"

#include <vulkan/vulkan.hpp>

namespace Vulkan {
    class Image {
    public:
        Image(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlagBits aspectFlags);
        ~Image();

        void release() const;

        [[nodiscard]] const VkImage& getImage() const;
        [[nodiscard]] const VkImageView& getImageView() const;
        [[nodiscard]] const VkDeviceMemory& getImageMemory() const;

        void createImageView(VkFormat format, VkImageAspectFlagBits aspectFlags, uint32_t mipLevels);

        void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) const;
        void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height) const;
        void generateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) const;
    private:
        Viking::Ref<Device> m_Device;

        VkImage m_Image{};
        VkDeviceMemory m_ImageMemory{};
        VkImageView m_ImageView{};
    };
}
