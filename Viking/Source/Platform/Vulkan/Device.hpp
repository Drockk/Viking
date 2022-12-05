#pragma once
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace Vulkan {
    class Device {
    public:
        Device(const Viking::Ref<PhysicalDevice>& physicalDevice);
        ~Device();

        [[nodiscard]] VkDevice get() const;
        std::tuple<VkQueue, VkQueue> getQueues();

        [[nodiscard]] VkCommandBuffer beginSingleTimeCommands() const;
        void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        //TEMP
        [[nodiscard]] VkCommandPool getCommandPool() const;

    private:
        void createCommandPool(const Viking::Ref<PhysicalDevice>& physicalDevice);

        VkDevice m_Device{};
        VkQueue m_GraphicsQueue{};
        VkQueue m_PresentQueue{};

        VkCommandPool m_CommandPool{};
    };
}
