#include "vipch.hpp"
#include "Platform/Vulkan/Image.hpp"

#include "Platform/Vulkan/Context.hpp"

namespace Vulkan {
    Image::Image(const uint32_t width, const uint32_t height, const uint32_t mipLevels, const VkSampleCountFlagBits numSamples, const VkFormat format, const VkImageTiling tiling, const VkImageUsageFlags usage, const VkMemoryPropertyFlags properties, VkImageAspectFlagBits aspectFlags) {
        m_Device = Context::get()->getDevice();

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(m_Device->get(), &imageInfo, nullptr, &m_Image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_Device->get(), m_Image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = Vulkan::Context::get()->getPhysicalDevice()->findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_Device->get(), &allocInfo, nullptr, &m_ImageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(m_Device->get(), m_Image, m_ImageMemory, 0);
    }

    Image::~Image() {
        release();
    }

    void Image::release() const {
        vkDestroyImageView(m_Device->get(), m_ImageView, nullptr);
        vkDestroyImage(m_Device->get(), m_Image, nullptr);
        vkFreeMemory(m_Device->get(), m_ImageMemory, nullptr);
    }

    const VkImage& Image::getImage() const {
        return m_Image;
    }

    const VkImageView& Image::getImageView() const {
        return m_ImageView;
    }

    const VkDeviceMemory& Image::getImageMemory() const {
        return m_ImageMemory;
    }

    void Image::createImageView(const VkFormat format, const VkImageAspectFlagBits aspectFlags, const uint32_t mipLevels) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_Image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        
        if (vkCreateImageView(m_Device->get(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}
