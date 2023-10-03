//
// Created by batzi on 03.10.2023.
//

#ifndef VIKING_VK_MESH_HPP
#define VIKING_VK_MESH_HPP

#include "vk_types.hpp"
#include <vector>
#include <glm/vec3.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

struct Mesh {
    std::vector<Vertex> m_vertices;

    AllocatedBuffer m_vertexBuffer;
};

#endif //VIKING_VK_MESH_HPP