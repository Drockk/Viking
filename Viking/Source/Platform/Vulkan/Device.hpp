#pragma once
#include "Platform/Vulkan/PhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>

namespace Vulkan {
    class Device {
    public:
        Device(const Viking::Ref<PhysicalDevice>& physicalDevice);
        ~Device();

        VkDevice get() const;
        std::tuple<VkQueue, VkQueue> getQueues();
    private:
        VkDevice m_Device{};
        VkQueue m_GraphicsQueue{};
        VkQueue m_PresentQueue{};
    };
}
