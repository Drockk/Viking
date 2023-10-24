//
// Created by batzi on 03.10.2023.
//

#ifndef VIKING_VK_MESH_HPP
#define VIKING_VK_MESH_HPP

#include "vk_types.hpp"
#include <vector>
#include <glm/vec3.hpp>

struct VertexInputDescription
{
    std::vector<VkVertexInputBindingDescription> m_bindings;
    std::vector<VkVertexInputAttributeDescription> m_attributes;

    VkPipelineVertexInputStateCreateFlags m_flags{};
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static VertexInputDescription getVertexDescription();
};

struct Mesh {
    std::vector<Vertex> m_vertices;

    AllocatedBuffer m_vertexBuffer;

    bool loadFromObj(const char* filename);
};

#endif //VIKING_VK_MESH_HPP
