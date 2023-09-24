#pragma once

#include "vk_types.hpp"

namespace vkinit
{
    VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t t_queueFamilyIndex, VkCommandPoolCreateFlags t_flags = 0);
    VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool t_pool, uint32_t t_count = 1, VkCommandBufferLevel t_level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderStageFlagBits t_stage, VkShaderModule t_shaderModule);
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo();
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo(VkPrimitiveTopology t_topology);
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo(VkPolygonMode t_polygonMode);
    VkPipelineMultisampleStateCreateInfo multisamplingStateCreateInfo();
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState();
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo();

    VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags t_flags = 0);
    VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags t_flags = 0);
}
