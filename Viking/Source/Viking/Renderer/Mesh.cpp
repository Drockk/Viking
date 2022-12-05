#include "vipch.hpp"
#include "Viking/Renderer/Mesh.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Viking {
    Mesh::Mesh(const std::string& filename) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string error;

        if (!LoadObj(&attrib, &shapes, &materials, &error, filename.c_str())) {
            throw std::runtime_error(error);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        for (const auto& [name, mesh] : shapes) {
            for (const auto& [vertex_index, normal_index, texcoord_index] : mesh.indices) {
                Vertex vertex{};

                vertex.pos = {
                    attrib.vertices[3 * vertex_index + 0],
                    attrib.vertices[3 * vertex_index + 1],
                    attrib.vertices[3 * vertex_index + 2]
                };

                vertex.texCoord = {
                    attrib.texcoords[2 * texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * texcoord_index + 1]
                };

                vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
                    m_Vertices.push_back(vertex);
                }

                m_Indices.push_back(uniqueVertices[vertex]);
            }
        }

        m_VertexBuffer = createRef<Vulkan::VertexBuffer>(m_Vertices);
        m_IndexBuffer = createRef<Vulkan::IndexBuffer>(m_Indices);
    }

    std::vector<Vertex>& Mesh::getVertices() {
        return m_Vertices;
    }

    std::vector<uint32_t>& Mesh::getIndices() {
        return m_Indices;
    }

    Ref<Vulkan::VertexBuffer> Mesh::getVertexBuffer() {
        return m_VertexBuffer;
    }

    Ref<Vulkan::IndexBuffer> Mesh::getIndexBuffer() {
        return m_IndexBuffer;
    }
}
