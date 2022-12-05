#pragma once

#include "Platform/Vulkan/Buffer.hpp"

namespace Viking {
    class Mesh {
    public:
        Mesh(const std::string& filename);

        std::vector<Vertex>& getVertices();
        std::vector<uint32_t>& getIndices();

        Ref<Vulkan::VertexBuffer> getVertexBuffer();
        Ref<Vulkan::IndexBuffer> getIndexBuffer();

    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;

        Ref<Vulkan::VertexBuffer> m_VertexBuffer;
        Ref<Vulkan::IndexBuffer> m_IndexBuffer;
    };
}
