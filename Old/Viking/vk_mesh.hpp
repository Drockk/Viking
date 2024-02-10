//
// Created by batzi on 03.10.2023.
//

#ifndef VIKING_VK_MESH_HPP
#define VIKING_VK_MESH_HPP

#include "vk_types.hpp"
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct VertexInputDescription
{
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;

    VkPipelineVertexInputStateCreateFlags flags = 0;
};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 uv;
    static VertexInputDescription get_vertex_description();
};

struct Mesh {
    std::vector<Vertex> vertices;

    AllocatedBuffer vertexBuffer;

    bool load_from_obj(const char* filename);
};

#endif //VIKING_VK_MESH_HPP
